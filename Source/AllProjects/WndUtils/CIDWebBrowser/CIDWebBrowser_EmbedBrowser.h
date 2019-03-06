#pragma once

#ifndef INEMBEDFILE
#ifdef __cplusplus
extern "C" {
#endif

long DisplayHTMLPage(void* pData, const wchar_t*);
//int DisplayHTMLStr(void* pData, const tCIDLib::TCh*);
int EmbedBrowserObject(void* pData, struct HWND__* hwndParent);
void DoPageAction(void* pData, const unsigned);
int GetShowAddrBar(void* pData);
int GetShowToolBar(void* pData);
int GetSilentMode(void* pData);
// void ReposBrowser(void* pData, const int, const int);
void ResizeBrowser(void* pData, const unsigned, const unsigned);
void SetShowAddrBar(void* pData, const unsigned);
void SetShowToolBar(void* pData, const unsigned);
void SetSilentMode(void* pData, const unsigned);
void UnEmbedBrowserObject(void* pData, struct HWND__* hwndParent);

#ifdef __cplusplus
}
#endif
#endif

#define WEBPAGE_GOBACK      0
#define WEBPAGE_GOFORWARD   1
#define WEBPAGE_GOHOME      2
#define WEBPAGE_SEARCH      3
#define WEBPAGE_REFRESH     4
#define WEBPAGE_STOP        5

