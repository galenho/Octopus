#include "http_help.h"

void print_hex(char* buff)
{
	for (int i=0;buff[i];i++)
		printf("%02x",(unsigned char)buff[i]);
	printf("\n");
}

unsigned char ToHex(unsigned char x)   
{   
	return  x > 9 ? x + 55 : x + 48;   
}  

unsigned char FromHex(unsigned char x)   
{   
	unsigned char y = 0;  
	if (x >= 'A' && x <= 'Z') y = x - 'A' + 10;  
	else if (x >= 'a' && x <= 'z') y = x - 'a' + 10;  
	else if (x >= '0' && x <= '9') y = x - '0';  
	else assert(0);  
	return y;  
}  

string UrlEncode(const string& str)  
{  
	std::string strTemp = "";  
	size_t length = str.length();  
	for (size_t i = 0; i < length; i++)  
	{  
		if (isalnum((unsigned char)str[i]) ||   
			(str[i] == '-') ||  
			(str[i] == '_') ||   
			(str[i] == '.') ||   
			(str[i] == '~'))
		{
			strTemp += str[i]; 
		}
		else if (str[i] == ' ')  
		{
			strTemp += "+";  
		}
		else  
		{  
			strTemp += '%';  
			strTemp += ToHex((unsigned char)str[i] >> 4);  
			strTemp += ToHex((unsigned char)str[i] % 16);  
		}  
	}

	return strTemp;  
}  

string UrlDecode(const string& str)  
{  
	string strTemp = "";  
	size_t length = str.length();  
	for (size_t i = 0; i < length; i++)  
	{  
		if (str[i] == '+') strTemp += ' ';  
		else if (str[i] == '%')  
		{  
			assert(i + 2 < length);  
			unsigned char high = FromHex((unsigned char)str[++i]);  
			unsigned char low = FromHex((unsigned char)str[++i]);  
			strTemp += high*16 + low;  
		}  
		else strTemp += str[i];  
	}  
	return strTemp;  
}

string make_md5(const std::string& str)
{
	unsigned char md[16] = {0};
	char tmp[3]={'\0'};
	char buf[32+1]={'\0'};

	MD5_CTX md5_ctx;    
	MD5_Init(&md5_ctx);
	MD5_Update(&md5_ctx, str.c_str(), str.length());
	MD5_Final((unsigned char*)md, &md5_ctx);  //获取MD5

	for (uint32 i = 0; i < 16; i++)
	{  
		sprintf(tmp,"%02X",md[i]); 
		strcat(buf,tmp);  
	}  

	string strTemp = (char*)buf;
	transform(strTemp.begin(),strTemp.end(),strTemp.begin(),::tolower);
	return strTemp;
}

string make_hmac(string& data, string& key)  
{  
	unsigned char result[64];  
	unsigned int res_len = 0; 

	HMAC_CTX* ctx = HMAC_CTX_new();  
	HMAC_CTX_reset(ctx);
	// Using sha1 hash engine here.  
	// You may use other hash engines. e.g EVP_md5(), EVP_sha224, EVP_sha512, etc  
	int key_len = strlen(key.c_str());
	HMAC_Init_ex(ctx, key.c_str(), key_len, EVP_sha1(), NULL);  
	int data_len = strlen(data.c_str());
	HMAC_Update(ctx, (unsigned char*)data.c_str(), data_len);  
	HMAC_Final(ctx, result, &res_len);  
	HMAC_CTX_free(ctx);  

	char sign_encode_str[64];
	memset(sign_encode_str, 0, sizeof(sign_encode_str));
	base64_encode((char*)result, res_len, sign_encode_str); 

	string strTemp = (char*)sign_encode_str;
	return strTemp;
}

int base64_encode(const char* in_str, int in_len, char *out_str)
{
	int size = 0;
	BIO * bmem = NULL;
	BIO * b64 = NULL;
	BUF_MEM * bptr = NULL;

	b64 = BIO_new(BIO_f_base64());

	BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
	bmem = BIO_new(BIO_s_mem());
	b64 = BIO_push(b64, bmem);
	BIO_write(b64, in_str, in_len);
	BIO_flush(b64);
	BIO_get_mem_ptr(b64, &bptr);

	size = bptr->length;
	memcpy(out_str, bptr->data, size);
	out_str[size] = 0;

	BIO_free_all(b64);

	return size;
}

int base64_decode(const char *in_str, int in_len, char *out_str)
{
	BIO *b64, *bio;
	int size = 0;

	if (in_str == NULL || out_str == NULL)
		return -1;

	b64 = BIO_new(BIO_f_base64());
	BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);

	bio = BIO_new_mem_buf(in_str, in_len);
	bio = BIO_push(b64, bio);

	size = BIO_read(bio, out_str, in_len);
	out_str[size] = '\0';

	BIO_free_all(bio);
	return size;
}

void sha256(const std::string &srcStr, std::string &encodedStr, std::string &encodedHexStr)  
{  
	// 调用sha256哈希    
	unsigned char mdStr[33] = {0};  
	SHA256((const unsigned char *)srcStr.c_str(), srcStr.length(), mdStr);  

	// 哈希后的字符串    
	encodedStr = std::string((const char *)mdStr);  
	// 哈希后的十六进制串 32字节    
	char buf[65] = {0};  
	char tmp[3] = {0};  
	for (int i = 0; i < 32; i++)  
	{  
		sprintf(tmp, "%02x", mdStr[i]);  
		strcat(buf, tmp);  
	}  
	buf[32] = '\0'; // 后面都是0，从32字节截断    
	encodedHexStr = std::string(buf);  
}

int rsa_verify(char *in, char *key_path, char* in2, int len)
{
	RSA *p_rsa;
	FILE *file;
	if((file=fopen(key_path,"r"))==NULL)
	{
		perror("open key file error");
		return 0;
	}

	if((p_rsa=PEM_read_RSAPublicKey(file,NULL,NULL,NULL))==NULL)
	{
		ERR_print_errors_fp(stdout);
		return 0;
	}
	if(!RSA_verify(NID_sha256,(unsigned char*)in,strlen(in),(unsigned char*)in2,len,p_rsa))
	{
		return 0;
	}
	RSA_free(p_rsa);
	fclose(file);
	return 1;
}

bool test_rsa_sign(string& content, string& sign)
{
	bool is_success = false;

	string encodedStr; 
	string encodedHexStr;
	sha256(content, encodedStr, encodedHexStr);

	char sign_encode_str[1024];
	memset(sign_encode_str, 0, sizeof(sign_encode_str));
	int sign_len = base64_decode(sign.c_str(), sign.length(), sign_encode_str);  

	//cout << "Base64 Decoded:" << endl << sign_encode_str << endl << endl; 

	string pub_key = 
		"-----BEGIN PUBLIC KEY-----\n"
		"MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAwEjyhZ9ZmaENkardtnCg\n"
		"w2M0JbNLGuvmQwezY5ML6TUAl6VCgHG+iaAf8nTyMXr6c2S4wvOBxFQiMTl48dFc\n"
		"xVmGmHc9Dfif6YVc2j9lxVDuPUzhGCC0xtibmkKjaLkQpNnuzScQw0bp8znH5MFT\n"
		"SBKfDBdsZoY1suWQNi3H/ChupeGkaSiLW6/GEvZWOrNQOD6ODVJ/gaO6W7BI0Dek\n"
		"v3zCXTLV6t/ahMsFmLin14faPWHcTGYFYT7UE59a9tpEs8UA1gu0IZEJDtzrzn/4\n"
		"C0C0E5MdcQkZ7j7SqCkPWvEC+6kX471VOHihGFR31MkLmPRDQQsPPbKidUj9o50C\n"
		"tQIDAQAB\n"
		"-----END PUBLIC KEY-----\n";

	BIO* memBIO = BIO_new(BIO_s_mem());  
	BIO_write(memBIO, pub_key.c_str(), pub_key.length());  
	RSA* rsa = PEM_read_bio_RSA_PUBKEY(memBIO, NULL, NULL, NULL);  
	if(!rsa)  
	{  
		return false;  
	}  

	int ret = RSA_verify(NID_sha256, (const unsigned char*)encodedStr.c_str(), encodedStr.length(), (const unsigned char*)sign_encode_str, sign_len, rsa);
	if (ret > 0) 
	{  
		// 正确
		is_success = true;
	}  
	else
	{
		is_success = false;
	}

	RSA_free(rsa);  
	BIO_free(memBIO);  

	return is_success;
}
