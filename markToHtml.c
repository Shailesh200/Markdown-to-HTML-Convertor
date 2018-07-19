#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char buffer[512];
void writeHeadElement(FILE *in, FILE *out);
void writeLinkElement(FILE *in, FILE *out);
void writeImgElement(FILE *in, FILE *out);
void writeElement(FILE *in, FILE *out);
void writeBlockQuoteElement(FILE *in, FILE *out);

int main(int argc, char* argv[]){

  FILE *in, *out;
  char ch;
  char *temp;
  
  if (argc<2){
    puts("too, few arguments\n");
    puts("Suggestion - possibly input file missing\n");
    exit(EXIT_FAILURE);
  }

  in = fopen(argv[1],"r");
  if (in==NULL){
    puts("Input File couldn't open\n");
    exit(EXIT_FAILURE);
  }

  out = fopen("output.html","w");
  if (out==NULL){
    puts("Output File couldn't created\n");
    exit(EXIT_FAILURE);
  }

  fputs("<html>\n<head>\n\t<title>My Doc</title>\n</head>\n<body>\n", out);
  while((ch=fgetc(in)) != EOF){
    if(ch=='#')
      writeHeadElement(in, out);
    else if(ch=='[')
      writeLinkElement(in, out);
    else if(ch=='!')
      writeImgElement(in, out);
    else if(ch=='*')
      writeElement(in, out);
    else if(ch=='>')
      writeBlockQuoteElement(in, out);
    else
      fputc(ch, out);
  }
  fputs("</body>\n</html>", out);
  
  fclose(in);
  fclose(out);
  printf("Successfully Translated..\n");
  return 0;
}

void writeHeadElement(FILE *in, FILE *out){
  int flag;
  int len;
  int count = 1;
  char ch, ch1;
  
  // counting Number of '#' Symbol
  flag = 1;
  while(flag){
    ch = fgetc(in);
    if(ch=='#'){
      count++;
      flag = 1;
    }else
      flag = 0;
  }
  ch1 = count+48;
  
  // skipping whiteSpaces
  flag=1;
  while(flag){
    ch = fgetc(in);
    if(ch==' ')
      flag = 1;
    else
      flag = 0;
  }
  
  fgets(buffer, 511, in);
  len = strlen(buffer);
  buffer[len-1] = '\0';
  fputs("<h", out);
  fputc(ch1, out);
  fputc('>', out);
  fputc(ch, out);             //First Char after WhiteSpace
  fputs(buffer, out);
  fputs("</h", out);
  fputc(ch1, out);
  fputs(">\n", out);
}

void writeLinkElement(FILE *in, FILE *out){
  char ch;
  int len;
  char url[100];

  buffer[0] = '\0';
  len = 0;
  while((ch=fgetc(in)) != ']')
    buffer[len++] = ch;
  buffer[len] = '\0';

  // skipping whiteSpaces
  while((ch=fgetc(in)) != '(');

  len = 0;
  while((ch=fgetc(in)) != ')')
    url[len++] = ch;
  url[len] = '\0';

  fputs("<a src=\'", out);
  fputs(url, out);
  fputs("\'>", out);
  fputs(buffer, out);
  fputs("</a>", out);
}

void writeImgElement(FILE *in, FILE *out){
  char ch;
  int len;
  char url[100];
  
  while((ch=fgetc(in)) != '[');
  
  buffer[0] = '\0';
  len = 0;
  while((ch=fgetc(in)) != ']')
    buffer[len++] = ch;
  buffer[len] = '\0';

  while((ch=fgetc(in)) != '(');

  len = 0;
  while((ch=fgetc(in)) != ')')
    url[len++] = ch;
  url[len] = '\0';

  fputs("<img src=\'", out);
  fputs(url, out);
  fputs("\' alt=", out);
  fputs(buffer, out);
  fputs(">", out);
}

void writeElement(FILE *in, FILE *out){
  long int position = ftell(in);
  char ch;
  int len;
  
  ch = fgetc(in);
  if(ch=='*'){
    buffer[0] = '\0';
    len = 0;
    while((ch=fgetc(in)) != '*')
      buffer[len++] = ch;
    buffer[len] = '\0';
    
    ch = fgetc(in);
    if(ch=='*'){
      // Verified Bold Syntax
      fputs("<b>", out);
      fputs(buffer, out);
      fputs("</b>", out);
    }else{
      // Not Bold Syntax, consider as Italics
      fputs("<i>*", out);
      fputs(buffer, out);
      fputs("</i>", out);
    }
  }
  else{
    // matching pattern
    while(1){
      ch = fgetc(in);
      if(ch=='\n' || ch=='*')
	break;
    }
    fseek(in, position, SEEK_SET);

    if(ch=='\n'){
      fputs("<ul>\n", out);
      do{
	buffer[0] = '\0';
	len = 0;
	while((ch=fgetc(in)) != '\n')
	  buffer[len++] = ch;
	buffer[len] = '\0';
	fputs("<li>", out);
	fputs(buffer, out);
	fputs("</li>\n", out);
	position = ftell(in);
      }while((ch=fgetc(in)) == '*');

      fseek(in, position, SEEK_SET);
      fputs("</ul>\n", out);
    }else{
      buffer[0] = '\0';
      len = 0;
      while((ch=fgetc(in)) != '*')
	buffer[len++] = ch;
      buffer[len] = '\0';

      fputs("<i>",out);
      fputs(buffer, out);
      fputs("</i>", out);
    }
  }
}

void writeBlockQuoteElement(FILE *in, FILE *out){
  char ch;
  fputs("<blockquote>", out);
  while((ch=fgetc(in)) != '\n'){
    if(ch=='*')
      writeElement(in, out);
    else
      fputc(ch, out);
  }
  fputs("</blockquote>", out);
}
