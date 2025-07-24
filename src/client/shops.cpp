// Regular Item Shops
// TODO: override GetShopVariation to call GetShopMetadata without processing the result (eliminate variations)
//+0x4420C0 __int64 __fastcall GetShopVariation(void*, int shopNum, char **pszShopTextureName);

//+0x441E40 void *__fastcall GetShopMetadata(void*, int shopNum, _DWORD *pNumEvents, char **pszShopTextureName);

// nIdx is the resource index and shop number (when 2 shops are on the same map, I think only Seian city)
// TODO: override this, and if pszType == "ISL" then return the data from our own shop definition based on ExteriorMapID and nIdx
//       first arg ignored
//+0x1B1770 void *__fastcall LoadRsc_0(void *pRscPackage, const char *pszType, int nIdx)

// Demon Fang Shops
// TODO: Override this and return the shop stocks directly based on ExteriorMapID, first arg ignored
//+0x43F5A0 okami::ItemShopStock *__fastcall cKibaShop__GetShopStockList(void*, _DWORD *numItems)

// TODO: Skill shop/dojo rando
// LoadRsc_0 called with "SSL", use ExteriorMapID
