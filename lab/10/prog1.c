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

 void make_key() {
 	int fout;
 	ssize_t ret;
 	
 	fout = open("secret", O_WRONLY | O_CREAT | O_TRUNC, 0600);
	 
 	ret = getrandom(&secret.key, BLOCK, 0);
 	if(ret != BLOCK) {
 		printf("random key generation failed\n");
 		abort();
 	}

 	ret = getrandom(&secret.iv, BLOCK/2, 0);
 	if(ret != BLOCK/2) {
 		printf("intialization vector generation failed\n");
 		abort();
 	}

 	write(fout, &secret, sizeof(secret));
 	close(fout);
 }

 int encrypt(unsigned char *plaintext, int length, unsigned char *ciphertext) {
 	int len;
 	int ciphertext_len;
 	EVP_CIPHER_CTX *ctx;

 	if(!(ctx = EVP_CIPHER_CTX_new())) handleErrors();
 	if(1 != EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, secret.key, secret.iv)) handleErrors();

 	if(1 != EVP_EncryptUpdate(ctx, ciphertext, &len, plaintext, length)) handleErrors(); 		
 	ciphertext_len = len;

 	if(1 != EVP_EncryptFinal_ex(ctx, ciphertext+len, &len)) handleErrors();
 	ciphertext_len += len;

 	EVP_CIPHER_CTX_free(ctx);
 	return(ciphertext_len);
 }
 
 int main(int argc, char **argv) {
 	unsigned char plain[BUFSIZ];
 	unsigned char *cipher;

 	int len;
 	int n;
 	int fout;
 
 	make_key();

 	bzero(&plain, BUFSIZ);
 	fgets((char*) plain, BUFSIZ, stdin);

 	n = strlen((char*) plain);
 	plain[n - 1] = 0;

 	n = (n/BLOCK + 1)*BLOCK;
 	cipher = (unsigned char *) malloc(n);
 	len = encrypt((unsigned char*) &plain, strlen((char*) plain)+1, cipher);
 	
 	printf("Cipher text: \n");
 	BIO_dump_fp(stdout, (const char*) cipher, len);
 	fout = open("message", O_WRONLY | O_CREAT | O_TRUNC, 0644);
	n = write(fout, cipher, len);

 	close(fout);
 }
