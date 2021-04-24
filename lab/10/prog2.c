 #include <sys/types.h>
 #include <sys/stat.h>
 #include <fcntl.h>
 #include <sys/random.h>
 #include <stdio.h>
 #include <openssl/conf.h>
 #include <openssl/evp.h>
 #include <openssl/err.h>
 #include <string.h>
 #include <unistd.h>

 #define BLOCK 32
 
  struct secretStruct {
 	unsigned char key[BLOCK];
 	unsigned char iv[BLOCK/2];
 } secret;

 void handleErrors() {
 	ERR_print_errors_fp(stderr);
 	abort();
 }

 void read_key() {
 	int fin;
 	
 	fin = open("secret", O_RDONLY, 0600);
 	read(fin, &secret, sizeof(secret));
 	close(fin);
 	
 }
 
 int decode(unsigned char *cipher, int length, unsigned char *plain) {
 	EVP_CIPHER_CTX *ctx;
 	int len;
 	int plaintext_len;
 	
 	if(!(ctx = EVP_CIPHER_CTX_new())) handleErrors();
 	if(1 != EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, secret.key, secret.iv)) handleErrors();

 	if(1 != EVP_DecryptUpdate(ctx, plain, &len, cipher, length)) handleErrors();
 	plaintext_len = len;

 	if(1 != EVP_DecryptFinal_ex(ctx, plain+len, &len)) handleErrors();
 	plaintext_len += len;

 	EVP_CIPHER_CTX_free(ctx);
 	return(plaintext_len);
 }
 
 int main(int argc, char ** argv) {
 	unsigned char buffer[BUFSIZ];
 	unsigned char *plain;

 	int len;
 	int n;
 	int fin;
 
 	read_key();

 	fin = open("message", O_RDONLY, 0644);
 	n = read(fin, buffer, BUFSIZ);
 	close(fin);

 	plain = (unsigned char *) malloc(n);
 	len = decode(buffer, n, plain);

 	printf("Plain text: %s\n", plain);	
 }
