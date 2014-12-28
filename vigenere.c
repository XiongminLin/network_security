/* Written by Xiongmin Lin <linxiongmin@gmail.com>, ISIMA, Clermont-Ferrand *
 * (c) 2014. All rights reserved.                                           *                                           
 * http://sancy.univ-bpclermont.fr/~guitton/enseignements/admin.html        */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

struct Same_Str{
  char str[1000];
  int  num;
  int  start[1000];
  int  end[1000];
};


double GetValueOft(unsigned int Num)
{
	double value[30] = {12.706, 4.303, 3.182, 2.766, 2.571, 2.447, 2.365, 2.308, 2.262, 2.228,
                             2.201, 2.179, 2.160, 2.145, 2.131, 2.120, 2.110, 2.101, 2.093, 2.086,
                             2.080, 2.074, 2.069, 2.064, 2.060, 2.056, 2.052, 2.048, 2.045, 2.042 };
	if(Num>0 && Num<=30){
		return value[Num-1];
	}else if(Num<=40){
		return 2.021;
	}
	else if(Num<=80){
		return 2.000;
	}else if(Num<=120){
		return 1.980;
	}else{
	return 1.960;
	}
}

void convert(char *origintext, char *plaintext)
{
  int i, j;
  i = 0;
  j = 0;
  memset(plaintext, '\0', strlen(plaintext));
  while(origintext[i] != '\0')
  {
    if(origintext[i] <= 'Z' && origintext[i] >= 'A')
    {
      plaintext[j] = origintext[i] + 32;
      j++;
    }
    if(origintext[i] <= 'z' && origintext[i] >= 'a')
    {
      plaintext[j] = origintext[i];
      j++;
    }
    i++;
  }
  
}
double anal_freq(char *text, double m_fre[26], double *Ke)
{
  int i, pos;
  int count;
  //double Ke;
  double fre[26] = {0.08167, 0.01492, 0.02782, 0.04253, 0.12702, 0.02228, 0.02015,
                    0.06094, 0.06966, 0.00153, 0.00772, 0.04025, 0.02406, 0.06749, 
                    0.07507, 0.01929, 0.00095,          0.05987, 0.06327, 0.09056,
                    0.02758, 0.00978, 0.02360,          0.00150, 0.01974, 0.00074, };

  /* the offset was set to verify that, no matter how    *
   * you pick up a letter in an offset interval, the     *
   * Ke results are always approximately equals to 0.067 */
  int offset      = 1;

  double variance = 0; // to estimate the quality of decryption text
  count = strlen(text);
  for(i = 0; i < 26; i++)
  {
    m_fre[i] = 0;
  }
  i = 0;
  while(text[i] != '\0' && i < strlen(text))
  {
    pos = (int)(text[i] - 'a');
    m_fre[pos]++;
    //i++;
    i = i + offset;
  }
  *Ke = 0;
  for(i = 0; i < 26; i++)
  {
   *Ke += (m_fre[i] * (m_fre[i] - 1));
  }
  count = count / offset;
  *Ke = *Ke / (double)(count * (count - 1));

 
  //printf("-------------------------------------------\n");
  for(i = 0; i < 26; i++)
  {
    m_fre[i] = m_fre[i] / (double)count;
    //char c = (char)(i + (int)('a'));
    //printf("-> %c: %f%%\n", c, m_fre[i] * 100);
  }
  //printf("The probability that two randomly chosen letters are the same: %2.4f\n",Ke);
  //printf("-------------------------------------------\n");

  /*calculate the variance*/  
  for(i = 0; i < 26; i++)
  {
    variance += pow((m_fre[i] - fre[i]), 2);
  }  
  variance = variance / (double)25;

  return variance;
}


void encrypt(char *plaintext, char *key, char *ciphertext)
{
  int    i   ;
  int    j   ;
  int offset ;
  i =    0   ;
  j =    0   ;

  while( plaintext[i] != '\n' &&
         plaintext[i] != '\0'   )
  {
    offset = key[j] -(int)('a');

    if(  plaintext[i] <= 'Z'  &&
         plaintext[i] >='A'     )
    {
      plaintext[i] = plaintext[i] + 32 ;
    }
    ciphertext[i] = (int)('a') + (plaintext[i] - (int)'a' + offset) % 26;
    i++;
    j++;
    if( key[j] == '\n' ||
        key[j] == '\0'   )
    {
      j = 0;
    }
  }
} 

void get_divisor(int *p, int num)
{
  int i;
  int j;
  for(i = 3; i < num; i++)
  {
    if(num%i == 0)
    {
      p[i-1] = 1;
    }
  }
  
}

/* to avoid noice, get_key_size() calculates the weight of each possible keys and the result is    *
 * more precise than get_key_size_02, which just calculates the same divisor of all possible keys */
/* the way i was used to calculate the weight was wrong, i should calculate the weight of same blocks, *
 * not divisors*/
int get_key_size(char* ciphertext)
{
  int size = 0;
  int i, j, m, k, r;
  int len = strlen(ciphertext);
  int *keysize = malloc(sizeof(int)*strlen(ciphertext));
  memset(keysize, 0, sizeof(int)*strlen(ciphertext));    // if possible key size is 27, set keysize[26]=1; 
  int malloc_size = 1000;
  struct Same_Str *same_str = malloc(sizeof(struct Same_Str) * malloc_size);
  int str_num;
  str_num = 0;
  for(i = 0; i < malloc_size; i++)
  {
    same_str[i].num = 0;
  }
   
  char *block_1 = malloc(len);
  char *block_2 = malloc(len);
  for(m = 2; m < len/2 ; m++)
  {
    int block_num = len -m;
    for(i = 0; i < block_num; i++)
    {
      memset(block_1, '\0', strlen(block_1));
      memcpy(block_1, ciphertext + i, m+1);
      int w = 0;                                           // weight
      for(j = i + 1 + m; j < block_num; j++)
      {
        if(j >= block_num) break;
        memset(block_2, '\0', strlen(block_2));
        memcpy(block_2, ciphertext + j, m+1);
        if(strcmp(block_1, block_2) == 0)
        {
           w++;
        
           int pos = same_str[str_num].num;
           same_str[str_num].start[pos] = i;
           same_str[str_num].end[pos]   = j;
           same_str[str_num].num++;

        }
      }
      if(w > 0)
      {
        sprintf(same_str[str_num].str, "%s", block_1);
        printf("No.%d, found same block: %s, %d times\n",str_num, same_str[str_num].str, same_str[str_num].num);
        str_num++;
        if(str_num >= malloc_size)
        {
          malloc_size += 1000;
          same_str = realloc(same_str, sizeof(struct Same_Str) * malloc_size);
        }
    
      }
      //usleep(100);
    }
  }

  free(block_1);
  free(block_2);



  /* from code above, we know all possible key sizes (count: str_num), which are stored in same_str array */

  /*if some strs whose replication time is more than 2, then it must be the REAL str*/
  k = 0;
  for(m = 0; m < str_num; m++ )
  {
    if(same_str[m].num >= 2)
    {
      /*the REAL str*/
      for(r = 0; r < same_str[m].num; r++)
      {
        int start  = same_str[m].start[r];
        int end    = same_str[m].end[r];
      	keysize[k] = end - start;
        k++;          
      }
    }
    else
    {
    
    }
  
  }


  /* get max number of key size, to malloc the array, each possible key size will be split as an   *
   * array ptr[i] and be added with an all-0 array: weight, from the weight of each key, we know   *
   * the most possible keysize                                                                     */
  int **ptr = malloc(sizeof(int*)*k);
  int max   = 0;
  for(i = 0; i < k; i++)
  {
     int num = keysize[i];
     max     = (max > num)? max : num;
  }

  int *weight = malloc(sizeof(int) * max);
  for(i = 0; i < max ;i++)
  {
    weight[i] = 0;
  }
  for(i = 0; i < k; i++)
  {
     int num       = keysize[i];
     ptr[i]        = malloc(sizeof(int)*max);
     ptr[i][num-1] = 1 ;
     memset(ptr[i], 0, max);

     /* at first, ptr[i] = "00000...00100..", after get_divisor, ptr[i] = "0..1..00..100..1..100...", *
     * where 1 means the match pos is the divisor number of keysize[i]                               */
     get_divisor(ptr[i], num);
     for(j = 0; j < max; j++)
     {
       weight[j] = weight[j] + ptr[i][j];
     }
  }
  int max_weight = 0;
  
  for(j = 0; j < max; j++)
  {
    if(weight[j] != 0)
    {
      printf("weight of %d: %d\n", j+1, weight[j]);
    }
    if(max_weight < weight[j])
    {
      size       = j+1;
      max_weight = weight[j];
    }
  }
  
  for(i = 0; i < k; i++)
  {
    free(ptr[i]);
  }  
  free(keysize);
  free(weight);
  free(ptr);
  free(same_str);
  return size;
}

void decrypt(char *ciphertext, char *key, char* plaintext)
{
  int    i   ;
  int    j   ;
  int offset ;
  i =    0   ;
  j =    0   ;
  while(ciphertext[i] != '\n' && ciphertext[i] != '\0')
  {
    offset = key[j] -(int)('a');
    if((ciphertext[i] - (int)'a' - offset) < 0)
    {
      plaintext[i] = ciphertext[i] - offset + 26;
    }
    else
    {
      plaintext[i] = ciphertext[i] - offset;  //it seems that we can't modify plaintext
    }
    i++;
    j++;
    if(key[j] == '\n' || key[j] == '\0')
    {
      j = 0;
    }
  }
  plaintext[i] = '\0';

}


char find_each_key_letter(char *cipher)
{
  int i;
  double variance[26];
  double Ke[26];
  char key;
  double Std_Ke = 0.067;
  double min = 10000; // set a very big value, to find a min
  double min_Ke_off = 1;
  char *plaintext = malloc(strlen(cipher) + 1);
  for(i = 0; i < 26; i++)
  {
    char prob_key[2];
    prob_key[0] = (char)(i + (int)('a'));
    prob_key[1] = '\0';
    double freq[26];
    decrypt(cipher, prob_key, plaintext);
    variance[i] = anal_freq(plaintext, freq, &Ke[i]);
    //printf("probable key letter : %s, Ke = %f, variance = %f \n",prob_key,  Ke[i], variance[i]);

    /*method 1: find the smallest variance*/
    if(variance[i]< min )
    {
      key = prob_key[0];
      min = variance[i];
    }

    /*method 2: find the closest Ke, this is wrong, because all the Ke are the same*/
    /*double off = (Ke[i] - Std_Ke) > 0 ? (Ke[i] - Std_Ke) : (Std_Ke - Ke[i]);
    if(off < min_Ke_off)
    {
      key = prob_key[0];
      min_Ke_off = off;
    }*/
  }
  free(plaintext);
  return key;  // find the most probable key latter c.

}

int main()
{
  //char *plaintext = "helloisimaiamveryhappytoliveinthisverybeautifulcampusthisisaverygoodplace";// if declare in this way , i can't modify it
  //char plaintext[100] = "helloisimaiamveryhappytoliveinthisverybeautifulcampusthisisaverygoodplace"; // OK
  char *origintext;
  char *plaintext;
  char *ciphertext;
  char **sepe_cipher;
  char *key;
  char *p_key;
  double fre[26];
  double Ke;
  int i, j, m;
  int keysize;
  origintext = (char*) malloc(1000);
  printf("please input origin text \n");
  for(i = 0; (*(origintext + i) = getchar()) != '\n'; i++)
  {
    if((i+1) % 1000 == 0)
    {
      origintext = (char*)realloc(origintext, strlen(origintext) + 1000);
    }
    
  }
  origintext[i] = '\0';
  plaintext = (char*)malloc(strlen(origintext));   
  memset(plaintext, '\0', strlen(plaintext));
  convert(origintext, plaintext);
  printf("frequency of plaintext: \n");
  anal_freq(plaintext, fre, &Ke);

  printf("please input the key\n");
  for(i = 0,key = (char*)malloc(100); (*(key + i) = getchar()) != '\n'; i++)
  {
    if((i + 1) % 100 == 0)
    {
      key=(char*)realloc(key,strlen(key)+100);
    }
  }
  *(key+i)='\0';
  
  ciphertext = malloc(strlen(plaintext) + 1);
  memset(ciphertext, '\0', strlen(ciphertext));
  
  encrypt(plaintext, key, ciphertext);
  printf("encrype: plaintext  = %s, key = %s\n", plaintext, key);
  printf("encrype: ciphertext = %s\n", ciphertext);

  printf("frequency of ciphertext: \n");
  anal_freq(ciphertext, fre, &Ke);


  
  keysize = get_key_size(ciphertext);
  printf("after calculating, keysize = %d\n",keysize);
  
  //keysize = 5;


  p_key = malloc(keysize+1);
  int len = 2 + strlen(ciphertext) / keysize;  // the length of each seperated ciphertext
  sepe_cipher = (char **) malloc(keysize * sizeof(char *));
  for(i = 0; i < keysize; i++)
  {

    /* set value for each seperated ciphertext*/
    sepe_cipher[i] = malloc(len);
    for(j = 0, m = 0; m < strlen(ciphertext); j++)
    {
      sepe_cipher[i][j] = ciphertext[m+i];
      m = m + keysize;
    }
    sepe_cipher[i][j] = '\0';

    /* find each most possible key letter (from a to z) for each seperated ciphertext*/

    p_key[i] = find_each_key_letter(sepe_cipher[i]);
    printf("find most probable key letter : %c\n", p_key[i]);
  }
  p_key[keysize] = '\0';

  //p_key = malloc(keysize+1);
  //printf("please inpute a possible key with a length of %d\n", keysize);
  //scanf("%s", p_key);
  //p_key[keysize] = '\0';


  decrypt(ciphertext, p_key, plaintext);
  printf("decrype: ciphertext = %s, probable key = %s \n", ciphertext, p_key);
  printf("decrype: plaintext  = %s\n", plaintext);
  
  free(origintext);
  free(plaintext);  
  free(ciphertext);
  free(key);
  free(p_key);
  for(i = 0; i < keysize; i++)
  {
    free(sepe_cipher[i]);
  }
  free(sepe_cipher);
  return 0;
}
