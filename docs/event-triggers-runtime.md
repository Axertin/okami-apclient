# Event triggers: runtime dispatch

The companion to [event-triggers.md](event-triggers.md), which describes the static SCA format on disk. This doc covers the runtime side: the per-stage TICK function, the state-bit dispatcher that fires stage-specific callbacks, and the patterns we figured out while bypassing Cave of Nagi's forced-tutorial softlock for AP randomization.

The descriptions here are concrete to Okami HD as analyzed via Ghidra; addresses use Ghidra's image-base convention (`0x180000000`). Where we say "main.dll +0xNNNNNN" the offset is module-relative.

## The big picture

Each stage has its own TICK function, called every frame from the main game loop. The TICK reads bits in a runtime "world-state" struct and dispatches stage-specific callbacks (cutscenes, tutorial steps, cleanup, etc.) when their gate bit is set and their anti-redispatch latch isn't. The callbacks run via the scheduler — they're not inline calls — so they get a scheduler frame and can do internal waits.

The bits themselves are flipped by other code (other callbacks, scripts, SCA trigger volumes the player crosses, the entrance cutscene's master script, etc.). Scripts and trigger volumes cause cascading bit flips that drive the chain.

## The world-state struct and bit encoding

The state lives in a struct accessed through a **pointer** at `main.dll + 0xB205C0`, named `DAT_180b205c0` in Ghidra. **It is a pointer variable, not a struct base.** Ghidra's pseudo-C makes this ambiguous — `DAT_180b205c0 + 0x39C` looks like field arithmetic but actually means "dereference the pointer and add `0x39C`".

To read state bits in mod code:

```cpp
auto worldStateBase = *reinterpret_cast<uintptr_t *>(mainBase + 0xB205C0);
auto bits = *reinterpret_cast<uint32_t *>(worldStateBase + 0x39C);
```

Reading `mainBase + 0xB205C0 + 0x39C` directly returns the uninitialized bytes of the pointer variable — always zero in our binary, which silently breaks any state polling that uses that address.

The per-stage state-bit setter is `FUN_180170830(uint32_t enc)`, paired with a clear-bit version `FUN_1801707c0(uint32_t enc)`. Both encode the target as:

```
addr = worldStateBase + (high16 * 0x20) + 0x35C + (low16/32) * 4
mask = 0x80000000 >> (low16 % 32)        // big-endian within the word
```

So `FUN_180170830(0x2001b)` (high16=2, low16=27) writes to `worldStateBase + 0x39C` with mask `0x10`. The big-endian bit numbering matches the in-game `okami::BitField<N>` convention.

The state struct is laid out as multiple sub-blocks of `0x20` bytes each, starting at `+0x35C`. For Cave of Nagi we mostly care about offsets `+0x39C` and `+0x3A0` (the second sub-block).

Heads-up on bitfield conventions: this state-bit setter and the engine's `okami::BitField<N>` both use big-endian-within-32-bit-word indexing. The brush "obtained" / "usable" bitfields manipulated by `FUN_18017c270` (`brushState+0x70` and `+0x78`) use the *opposite* convention: little-endian within a 64-bit word (`1ULL << bit`). The mod's `apgame::obtainedBrushesSource` reads them via byte-and-bit math accordingly. Don't transpose between the two.

## The per-stage TICK

Cave of Nagi's TICK is `FUN_1804c4300`. The version below is abridged to the entrance + constellation/kami section that the AP softlock fix touches. The real function continues with four more dispatch sections (gating `FUN_1804c30b0`, `FUN_1804c1050`, `FUN_1804c3c00`, `FUN_1804c3860`) that handle later CoN events; check Ghidra for the full body.

```c
if ((world+0x39C >> 0x1d) & 1) {
    // entrance dispatch section (FUN_1804c1b10)
    if (!(world+0x39C & 0x200) /* entrance latch, mask 0x200 */) {
        if (FUN_1803f3380(ctx, 0, 0, 1) && !(GGS[0] & 0x40000000)) {
            FUN_180170830(0x20009);  // entrance dispatch latch (+0x39C bit 9, mask 0x00400000)
            FUN_1801707c0(0x5);      // clear +0x35C bit 5
            FUN_1803f4900(ctx);
            ret = FUN_1803ef3c0(ctx + 0x20, FUN_1804c1b10, 0xffffffff);
            FUN_1803f3170(ctx, ret);
        }
    }
    /* atomic clear of +0x39C bit 17 (mask 0x4000) */
    if (FUN_1803f3380(ctx, 1, 0, 1)) FUN_180170830(0x20011);
    if (!(world+0x3A0 & 0x400000)) FUN_1804c14a0();
}
if ((world+0x39C >> 0x1c) & 1) {
    // constellation/kami/tutorial section
    if ((world+0x3A0 & 0x400000) && !(world+0x39C & 0x400)) {
        FUN_180170830(0x20015);  // FUN_1804c31c0 anti-redispatch latch
        FUN_1803f4900(ctx);
        ret = FUN_1803ef3c0(ctx + 0x20, FUN_1804c31c0, 0xffffffff);
        FUN_1803f3170(ctx, ret);
    }
    FUN_1804c1900();   // constellation puzzle handler
    if (!(world+0x39C & 0x200) && (world+0x3A0 & 0x200000)) {
        FUN_180170830(0x20016);
        FUN_1803f4900(ctx);
        ret = FUN_1803ef3c0(ctx + 0x20, FUN_1804c1f50, 0xffffffff);
        FUN_1803f3170(ctx, ret);
    }
    FUN_1804c1130();   // inner TICK for input + later dispatch gates
    FUN_1804c1820();
}
// ... further sections for FUN_1804c30b0, FUN_1804c1050, etc.
```

Two patterns matter here.

### Pattern 1: gate + anti-redispatch latch + dispatch

Most callback dispatches follow the same shape:

```c
if (GATE_BIT_SET && !LATCH_BIT_SET) {
    FUN_180170830(LATCH_ENC);                      // set the latch
    FUN_1803f4900(ctx);                            // enter cutscene mode
    ret = FUN_1803ef3c0(ctx + 0x20, callback, ~0); // schedule the callback
    FUN_1803f3170(ctx, ret);                       // register it
}
```

The latch is what makes a one-shot of an otherwise per-tick check. `FUN_1803f4900` sets a bunch of bits (see below) before scheduling, so the callback runs in "cutscene mode."

Pattern variant: a few dispatch sites omit the latch entirely. The clearest example is `FUN_1804c1130`'s third clause (the `FUN_1804c2560` post-tutorial-cleanup dispatch), which fires every tick that bit 29 of `+0x39C` is set. The callback itself must clear the gate before yielding, or the scheduler must dedupe pending dispatches; either way, the "every dispatch follows the same shape" rule has exceptions, and you should read the actual TICK before assuming a latch will be there.

### Pattern 2: cutscene-mode entry/exit

`FUN_1803f4900` does:

```
AreaLoadFlags (main.dll+0xB6B2A0) |= 0x02802000   // letterbox + scene
GGS[0]        (main.dll+0xB6B2AC) |= 0x40000040   // cutscene mode
GGS[2]        (main.dll+0xB6B2B4) &= ~0x01000000  // ENABLES player input (yes, really)
```

The opposite is `FUN_1803f4730(ctx, 0)`, which clears roughly the same `AreaLoadFlags` and `GGS[0]` bits. It is *not* a precise inverse: it clears `GGS[0] & ~0x40000042` (an extra bit 1) and `AreaLoadFlags & ~0x00802000` unconditionally with `~0x02000000` cleared only when `GGS[0] & 0x02000000` is already clear. For bypass purposes, replicating the exact `FUN_1803f4900` masks is what you actually need; don't try to mirror `FUN_1803f4730` byte-for-byte. The standard wrapper `FUN_1803f48d0(ctx, 1)` calls `FUN_1803f4730` and then `FUN_1804561d0(*(ctx+0x30))` (a scheduler-frame release).

The camera entity reads these bits to decide whether to stay in its cutscene pose or follow the player. Bypass code that leaves the bits set keeps the camera stuck.

## The scheduler-frame caveat

Callbacks scheduled via `FUN_1803ef3c0(ctx + 0x20, fn, flags)` receive a scheduler frame at `ctx + 0x30` that they use for their internal waits (`FUN_1803f5170`, `FUN_1804567c0`). **A direct call to such a callback — bypassing `FUN_1803ef3c0` — crashes** as soon as the callback dereferences `ctx + 0x30`, because either (a) the field is null/stale or (b) it points at an old scheduler frame from a different callback's lifetime.

If you need to dispatch a callback from outside the natural chain, replicate the scheduler dance instead of calling the function directly:

```cpp
auto fnSchedule = (ScheduleCbFn)(mainBase + 0x3EF3C0);
auto fnRegister = (RegisterCbFn)(mainBase + 0x3F3170);
uint32_t ret = fnSchedule(ctx + 0x20, callbackPtr, 0xFFFFFFFFU);
fnRegister(ctx, ret);
```

The callback runs on the next scheduler pass with a proper frame. Internal waits pump correctly. This is how you "force-fire" an event without triggering the natural gate chain.

## Inlining pitfalls (unverified)

Small wrapper functions are inline candidates and may not show up as call targets in every caller. The original observation here was that a hook on `FUN_1803f35f0` produced zero `schedule_script` events for scripts queued from `FUN_1804c1f50` (e.g. CoN's tutorial script `0x1010`), but static analysis disagrees: Ghidra shows `FUN_1804c1f50` at `0x1804c224b` has an `UNCONDITIONAL_CALL` to `FUN_1803f35f0`, and the decompiled body contains a literal `FUN_1803f35f0(ctx, 0x1010, ...)` call site. 

## The skip-the-first-link pattern

When a callback chain has multiple stages — cutscene → puzzle → dialog → tutorial → cleanup — you can in theory hook any of them. In practice, hooking anything but the first one usually fails, because by then:

- The camera has been switched to a special cutscene pose.
- Cutscene-mode bits are set across multiple addresses.
- One or more scripts have started executing concurrently, holding scheduler resources.
- Internal counters (`ctx+4`, `ctx+8`, ...) have been initialized.

Reversing all of that mid-chain requires either calling the natural cleanup function (which has scheduler dependencies that don't hold outside its expected context) or replicating its bit operations by hand (which we couldn't get right without missing some entity-level state).

The clean fix is to hook the **first** callback in the chain and replace it with a stub that:

1. Clears the cutscene-mode bits the dispatcher's `FUN_1803f4900` pre-call set.
2. Replicates the one essential side-effect we still need (in CoN's case, granting the brush bit so `BrushMan` fires the AP check).
3. Returns without calling the original — none of the downstream dispatches happen because their gate bits never get set.

The entrance-hook on `FUN_1804c1b10` works precisely because it's the first link. The same idiom applied to `FUN_1804c31c0` (statue rejuv → constellation → ...) skipped the entire CoN forced-tutorial chain in one move, after multiple iterations trying to bypass `FUN_1804c1f50` (the middle of the chain) had failed.

## Cave of Nagi callback chain (reference)

Confirmed offsets and dispatch order:

| Function        | Offset      | Role                                                                                                                                                  |
| --------------- | ----------- | ----------------------------------------------------------------------------------------------------------------------------------------------------- |
| `FUN_1804c4300` | `+0x4C4300` | per-stage TICK (outer dispatcher)                                                                                                                     |
| `FUN_1804c1b10` | `+0x4C1B10` | entrance cutscene (camera lower + portcullis fall + script `0x1003`)                                                                                  |
| `FUN_1804c31c0` | `+0x4C31C0` | statue rejuv: brush bit `0xC` via `FUN_18017c270`, constellation puzzle                                                                               |
| `FUN_1804c1900` | `+0x4C1900` | constellation puzzle handler (per-tick, increments `ctx+4`)                                                                                           |
| `FUN_1804c1f50` | `+0x4C1F50` | kami dialog + forced rock-slash tutorial (script `0x1010`)                                                                                            |
| `FUN_1804c1130` | `+0x4C1130` | inner TICK (rock-slash input check, dispatches `FUN_1804c28d0` and `FUN_1804c2560` based on `+0x39C` bits)                                            |
| `FUN_1804c28d0` | `+0x4C28D0` | rock-slash callback (player slashed: schedules script `0x1012` rock break)                                                                            |
| `FUN_1804c2560` | `+0x4C2560` | post-tutorial cleanup (kami leaves, area unblocks)                                                                                                    |
| `FUN_1804c2770` | `+0x4C2770` | camera-reset cleanup (called from `FUN_1804c2560` epilogue: clears letterbox, calls `FUN_180152200(camera, 2, 2, ...)` to switch back to follow mode) |
| `FUN_1803f4900` | `+0x3F4900` | enter cutscene mode (called by dispatcher pre-call)                                                                                                   |
| `FUN_1803f4730` | `+0x3F4730` | exit cutscene mode (clears bits set by `FUN_1803f4900`)                                                                                               |
| `FUN_1803f48d0` | `+0x3F48D0` | wrapper: `FUN_1803f4730` + scheduler-frame release                                                                                                    |
| `FUN_1803ef3c0` | `+0x3EF3C0` | scheduler enqueue (sets up the frame at `ctx + 0x30`)                                                                                                 |
| `FUN_1803f3170` | `+0x3F3170` | scheduler register (after `FUN_1803ef3c0`)                                                                                                            |
| `FUN_180170830` | `+0x170830` | state-bit setter (encoded address)                                                                                                                    |
| `FUN_1801707c0` | `+0x1707C0` | state-bit clearer (encoded address)                                                                                                                   |
| `FUN_18017c270` | `+0x17C270` | brush-bit operation (`brushState`, `bitIdx`, `op`)                                                                                                    |
| `FUN_18048c720` | `+0x48C720` | "transition to stage N" entry point                                                                                                                   |
| `FUN_18048d140` | `+0x48D140` | stage-id -> stage record lookup                                                                                                                       |
| `FUN_18048c8c0` | `+0x48C8C0` | install descriptor hook at `+0x38`                                                                                                                    |
| `FUN_18048c8e0` | `+0x48C8E0` | install descriptor hook at `+0x40`                                                                                                                    |
| `FUN_18048c8f0` | `+0x48C8F0` | install descriptor hook at `+0x48`                                                                                                                    |
| `FUN_1804c4940` | `+0x4C4940` | CoN's stage init function (installs the three lifecycle hooks)                                                                                        |
| `DAT_1807aa1b0` | `+0x7AA1B0` | master stage record table (16 bytes/entry)                                                                                                            |
| `DAT_180b65ed0` | `+0xB65ED0` | active stage descriptor                                                                                                                               |

Dispatch gates within `+0x39C` (mask convention: BitField bit N = `0x80000000 >> N`):

| Bit | Mask         | Meaning                                                                                  |
| --- | ------------ | ---------------------------------------------------------------------------------------- |
| 9   | `0x00400000` | latch: `FUN_1804c1b10` already dispatched (set by `FUN_180170830(0x20009)`)              |
| 17  | `0x00004000` | atomic-cleared near top of TICK by `FUN_180170830(0x20011)`; meaning unconfirmed         |
| 21  | `0x00000400` | latch: `FUN_1804c31c0` already dispatched                                                |
| 22  | `0x00000200` | also gates the entrance dispatch as a "do not re-fire" check                             |
| 24  | `0x00000080` | "kami sequence done" — gates inner TICK input check                                      |
| 25  | `0x00000040` | "kami appearing" — gates `FUN_1804c2ac0` dispatch                                        |
| 26  | `0x00000020` | latch for bit-25 dispatch                                                                |
| 27  | `0x00000010` | "rock slashed" — gates `FUN_1804c28d0` dispatch                                          |
| 28  | `0x00000008` | latch for bit-27 dispatch                                                                |
| 29  | `0x00000004` | "tutorial done" — gates `FUN_1804c2560` dispatch (no latch; see Pattern 1 variant above) |

The bit-22 (`0x00000200`) entry is reused: the same bit acts as a gate-clear check before the entrance dispatch and as the latch for `FUN_1804c1f50` later in the TICK. Same address and bit, two different roles depending on which TICK section is reading it.

Other state words touched by CoN's TICK chain:

| Address  | Bit / mask           | Meaning                                                                      |
| -------- | -------------------- | ---------------------------------------------------------------------------- |
| `+0x35C` | bit 5 / `0x04000000` | cleared by `FUN_1801707c0(5)` in the entrance dispatch (purpose unconfirmed) |

Dispatch gates within `+0x3A0`:

| BitField bit | Mask         | Meaning                                      |
| ------------ | ------------ | -------------------------------------------- |
| 41 (low=41)  | `0x00400000` | "rejuv pending" — gates `FUN_1804c31c0`      |
| 42 (low=42)  | `0x00200000` | "constellation done" — gates `FUN_1804c1f50` |

## Kamiki Village Water Lily chain (reference)

Confirmed offsets and dispatch order for the Sakuya descent + Water Lily grant + forced lily-pad tutorial. Hooked the same way as CoN: replace `FUN_1804c64a0` with a stub that grants the brush and jumps the stage descriptor to sub-state `0xe`.

| Function        | Offset      | Role                                                                                                                                                  |
| --------------- | ----------- | ----------------------------------------------------------------------------------------------------------------------------------------------------- |
| `FUN_1804c64a0` | `+0x4C64A0` | first link (no static xrefs; TICK-dispatched). Steps stage descriptor 10 -> 11 -> 12 via `FUN_18048c720`, pokes worldStateBase `+0x3cc` bit 4 and `+0x3bc` bit `0x10`, schedules `FUN_1804c7d80`. |
| `FUN_1804c7d80` | `+0x4C7D80` | Sakuya descent + camera/UI fade ramp. Sets `+0x35C` bits, transitions descriptor to `0xd`, schedules `FUN_1804ca430`.                                 |
| `FUN_1804ca430` | `+0x4CA430` | Water Lily grant: `FUN_18017c270(brushState, 5)`. Plays grant animation; runs `FUN_1803f5170(.., FUN_1804ca860, ..)` to enter the tutorial loop; queues scripts `0x10f5`, `0x10f6`. |
| `FUN_1804ca860` | `+0x4CA860` | lily-pad practice loop. Success path transitions stage descriptor to `0xe` via `FUN_18048c720` and calls `FUN_180170690(brushState)`.                 |
| `FUN_18048c720` | `+0x48C720` | "transition to sub-state N" entry point (also documented in the CoN reference above)                                                                  |

The bypass installed by `src/okami-apclient/eventfix/kamiki_village.cpp` replaces `FUN_1804c64a0` with a stub that calls `clearCutsceneModeBits()`, `grantBrush(BrushOverlay::water_lily)`, and `transitionStageSubState(0xe)`. The natural chain's per-map state writes (e.g. `KamikiVillage` worldStateBits 11 / 149 / 163) are set elsewhere (trigger volumes, post-bloom scripts); the bypass does not interact with them.

## Hana Valley Guardian Sapling Dialog
After starting the sequence in Hana Valley's guardian sapling, there's a dialog that prevents you from leaving the area. It's handled by FUN_1804d35d0:
```cpp
  if ((((DAT_180b6b2ac >> 0x1e & 1) == 0) &&
      ((*(uint *)(WORLD_STATE_POINTER + 0x3dc) >> 0xd & 1) != 0)) &&
     ((*(uint *)(WORLD_STATE_POINTER + 0x3dc) & 2) == 0)) {
    cVar2 = FUN_1803f3380(PTR_DAT_1807a8cb0,0x11,0,1); // WILD GUESS: This checks the text that's left to display? 
    if (cVar2 == '\0') {// If none, the state is cleared.
      clear_world_state_bit(0x40032); // => FUN_1801707c0 
      return;
    }
    // Checks if we're in the guardian sapling sequence
    if ((*(uint *)(WORLD_STATE_POINTER + 0x3e0) & 0x2000) == 0) {
      set_world_state_bit(0x40032); => //=> FUN_180170830, set the flag for issun's dialog
      puVar1 = PTR_DAT_1807a8cb0;
      uVar3 = FUN_1803ef3c0(PTR_DAT_1807a8cb0 + 0x20,FUN_1804d9490,0xffffffff); // Callback scheduler
      FUN_1803f3170(puVar1,uVar3);
      return;
    }
  }
  return;
}
```
We simply replace with with an empty function to never prevent the player from leaving the area.


## Hana Valley Bloom Tutorial
The handler for sunrise usage in the sapling room is FUN_184D2410:
```cpp
  wk::math::cVec::cVec(local_48);
  wk::math::cVec::cVec(local_38);
  wk::math::cVec::cVec(local_28);
  wk::math::cVec::cVec(local_18);
  if ((((*(byte *)(WORLD_STATE_POINTER + 0x3dc) & 2) == 0) &&
      (iVar3 = FUN_1801690c0(&DAT_1808909c0,local_58,0xffffffff,0), iVar3 == 2)) &&
     (iVar3 = FUN_180169ec0(&DAT_1808909c0), iVar3 == 1)) {
    DAT_180b6b2ac = DAT_180b6b2ac | 0x40000000;
    LOCK();
    UNLOCK();
    wk::math::cVec::cVec(local_68,(cVec *)&DAT_180b66390);
    if ((1.0 < local_64) || (local_64 < -1.0)) {
      if ((*(uint *)(WORLD_STATE_POINTER + 0x3dc) & 0x100) != 0) {
        set_world_state_bit(0x40031); // Flag 49, this is the branch if you use sunrise without having the crystal in placxe
        puVar1 = PTR_DAT_1807a8cb0;
        uVar4 = schedule_callback(PTR_DAT_1807a8cb0 + 0x20,FUN_1804d9490,0xffffffff);
        register_callback(puVar1,uVar4);
        return;
      }
    }
    else {
      if ((*(byte *)(WORLD_STATE_POINTER + 0x3dc) & 0x20) != 0) {
        set_world_state_bit(0x4001e); // Flag 30, this is the branch if you do have the crystal in place and use sunrise
        puVar1 = PTR_DAT_1807a8cb0;
        uVar4 = schedule_callback(PTR_DAT_1807a8cb0 + 0x20,FUN_1804D60E0,
                                  0xffffffff); // Schedule Callback to the curtscences
        register_callback(puVar1,uVar4);
        return;
      }
      cVar2 = FUN_1803f3380(PTR_DAT_1807a8cb0,5,0);
      puVar1 = PTR_DAT_1807a8cb0;
      if (cVar2 != '\0') {
        uVar4 = schedule_callback(PTR_DAT_1807a8cb0 + 0x20,FUN_1804d53f0,0xffffffff);
        register_callback(puVar1,uVar4);
        return;
      }
    }
    FUN_18014a360(1);
  }
```

Our replace first link in approach would have use replace FUN_1804D60E0 with a stub; But this cutscene chain loads in the unbloomed guardian tree. This load doesn't get persisted anywhere while it hasn't been bloomed, meaning reloading the map will remove it forerver, preventing the player form accessing Healed Hana Valley.(*This is likely why the above handler was added to prevent the player from exiting the map.*)

Even is we had a stub that would load in the tree, we'd need the player to have bloom when doing this chain of events to unlock healed Hana Valley, otherwise they'd be locked off it forever. 

## Stage architecture (one level above TICK)

The CoN TICK has zero static call sites in main.dll. Its only xref is its `.pdata` exception-unwind entry. Despite that, scripts and TICKs in this engine are *not* loaded from external files: there is no separate script bytecode language. Scripts are compiled C++ callbacks baked into main.dll, scheduled by ID via `FUN_1803f35f0(ctx, script_id, ...)` and similar. So the TICK gets installed at runtime through some indirection that we haven't fully traced statically (likely a stage-id -> function-pointer table populated during stage init), but the answer lives somewhere inside main.dll, not in an on-disk script file. The on-disk room files (`data_pc/stN/rXXX.bin`) carry SCA trigger volumes, MSD strings, and per-room asset data, not callback pointers. That said, the *parallel* machinery (the stage descriptor object the TICK eventually drives) is fully visible in the binary.

### Master stage record table

`DAT_1807aa1b0` (`main.dll + 0x7AA1B0`) is a flat array of 16-byte records, indexed by stage id:

```c
struct StageRecord {
    uint16_t script_id;     // e.g. 0x111e for Cave of Nagi
    uint16_t pad;
    uint32_t flags_or_param;
    void*    stage_init_fn; // installs the descriptor's lifecycle hooks
};
```

The lookup is `FUN_18048d140(descriptor, stage_id)`, and the higher-level "transition to stage N" entry point is `FUN_18048c720(descriptor, stage_id)`. Cave of Nagi sits at index 5 (`script_id=0x111e`, `stage_init_fn=FUN_1804c4940`).

### Active stage descriptor

`DAT_180b65ed0` is the global stage descriptor with three function-pointer slots installed by the per-stage init function:

| Offset  | Setter          | Meaning                                     |
| ------- | --------------- | ------------------------------------------- |
| `+0x38` | `FUN_18048c8c0` | small per-stage hook (lifecycle / pre-tick) |
| `+0x40` | `FUN_18048c8e0` | small per-stage hook                        |
| `+0x48` | `FUN_18048c8f0` | small per-stage hook                        |

CoN's `FUN_1804c4940` installs `LAB_1804c48d0` / `DAT_1804c4910` / `LAB_1804c4920` into these slots. These callbacks are tiny (≤30 bytes each) and are *not* the per-frame TICK; they are descriptor-lifecycle hooks. The TICK at `FUN_1804c4300` is dispatched separately, through some in-binary indirection we haven't fully resolved (the function has no static xrefs other than `.pdata`).

There are around 318 references to `DAT_180b65ed0` across the binary, so this descriptor is the central object the rest of the per-stage machinery is built around. Calls to `FUN_18048c720(&DAT_180b65ed0, N)` with small N (we see `0x2a`, `0x55`, `0x94` in the wild) are intra-stage transitions to sub-state IDs.

### What this means for bypass work

The TICK and the callbacks it dispatches are all compiled into main.dll, so MinHook-ing them is the natural intervention point. There is no script-file format to patch separately; "scripts" in this engine are compiled C++ callbacks dispatched by ID, with the ID acting as an index into an in-binary table. The bits driving those dispatches are flipped by SCA trigger volumes from the room file *and* by other in-binary callbacks; the room file influences the chain by deciding *when* the player crosses a trigger zone, but the resulting handler is always baked-in code.

For future bypass work, that means: hook the first link in the in-binary callback chain (as `confix.cpp` does), or, if the goal is per-room rather than per-event control, you could patch the SCA section of the room file to suppress the trigger zone that flips the gating bit in the first place. Both routes stay inside known, statically-analyzable territory; neither requires reverse-engineering an external script format because there isn't one. This section is here so the next person doesn't waste time looking for static call sites to the TICK that aren't there.
