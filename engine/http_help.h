#ifndef _HTTP_HELP_H_
#define _HTTP_HELP_H_

#include "common.h"
 
#include "openssl/md5.h"    
#include "openssl/sha.h"   
#include "openssl/hmac.h"
#include "openssl/des.h"    
#include "openssl/rsa.h"    
#include "openssl/pem.h"    
#include "openssl/bio.h"
#include "openssl/err.h"


#define MSG_LEN 1024

#define KEY_LENGTH  2048             // 密钥长度  
#define PUB_KEY_FILE "public.rsa"    // 公钥路径  
#define PRI_KEY_FILE "private.rsa"   // 私钥路径 


string UrlEncode(const string& str);
string UrlDecode(const string& str);

string make_md5(const string& str);
string make_hmac(string& data, string& key);

const char base[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=";  

int base64_encode(const char* in_str, int in_len, char *out_str);
int base64_decode(const char *in_str, int in_len, char *out_str);

// sha256摘要哈希   
void sha256(const string &srcStr, string &encodedStr, string &encodedHexStr); 

int rsa_verify(char *in, char *key_path, char* in2, int len);
int rsa_sign(char *in, char *key_path, char* out, int* plen);

// 函数方法生成密钥对   
void generateRSAKey(string strKey[2]);

bool test_rsa_sign(string& content, string& sign);

#endif //_HTTP_HELP_H_