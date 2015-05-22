// compiler command: arm-linux-gcc -static -o testmplayer testmplayer.c
// This program will search vlist.txt file for testing all video files in this txt file.
// the video file can be placed in any folder below or the same as the folder of vlist.txt
// And, it will generate a log file as command line assiged.
//----------------------------------------------------------
// vlist.txt content:
// video filename, time
//----------------------------------------------------------
// They are separated by "," AND, THIS FILE CAN'T BE UNICODE OR UTF-8..... ASCII is perferred...
// utf-8 is also ok, but please keep a "# empty this line" at the 1st line of vlist.txt


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/time.h>
#include <fcntl.h>
#include <locale.h>
//#define DEBUG_ON_PC     1
#define MAXFLDS 200     /* maximum possible number of fields */
#define MAXFLDSIZE 256   /* longest possible field + 1 = 31 byte field */
#define SKYFB_SET_MODE_ONLY   0xffff101d

#ifdef DEBUG_ON_PC
char mntpath[1024]="./mnt";
#else
char mntpath[1024]="/mnt";
#endif
char vlistname[20]="vlist.txt";

void parse( char *record, char *delim, char arr[][MAXFLDSIZE],int *fldcnt)
{
    char*p=strtok(record,delim);
    int fld=0;
    
    while(p)
    {
        strcpy(arr[fld],p);
				fld++;
				p=strtok('\0',delim);
	  }		
	*fldcnt=fld;
}
//return 1 is found, 0 is not. *path is input, *vlistpath is output.
int search_file(char *lookfile, char *path, char* filepath)
{
	  DIR *dir;
    struct dirent * ptr;
    int i;
    dir =opendir(path);
    if ( dir == NULL )
    {
    	   printf(" fail to open %s\n", path );
    	   exit (1);
    }
    while((ptr = readdir(dir))!=NULL) {
        char pathname[1024];
        struct stat buf;
        
        sprintf(pathname,"%s/%s", path, ptr->d_name);
        stat(pathname, &buf);

        if(S_ISREG(buf.st_mode))
        {
        		if (strcasecmp(lookfile, ptr->d_name) == 0)
        			{ // we found it!!!!!
        				 strcpy(filepath, path);
        				 strcpy(lookfile, ptr->d_name); // for case problem.
        				 closedir(dir);
        				 return 1;
        			}
        }
        if(S_ISDIR(buf.st_mode))
        {
        		if(strcmp(".", ptr->d_name) != 0 && strcmp("..", ptr->d_name) != 0)
        		{
           		 printf("%s: directory\n", ptr->d_name);
           		 // now, the pathname is what we need to search for vlist.txt.
           		 if (search_file(lookfile, pathname, filepath) == 1 )
           		 	{ // found it from subdirectory
           		 		  closedir(dir);
           		 			return 1;
           		 	}
           	}
        }    
    }
    closedir(dir);
    return 0;
}

int main(int argc, char *argv[])
{
	char outputfile[20];
	struct timeval tv, tv_start, tv_last;
	char tmp[1024]={0x0};
	int fldcnt=0;
	char arr[MAXFLDS][MAXFLDSIZE]={0x0};
	int recordcnt=0;	
	FILE *in, *out;                      /* open file on command line */
	char vlistpath[1024];
	int  tolerance;
	int  pid, status, recordall=0;
	int	 pass=0, fail=0, total=0, miss=0;
	
	printf("Skyviia MPlayer test program v1.10 (20100610)\n");
	if (argc < 3 )
	{
			printf("usage: testmplayer outputfile tolerance [recordall]\nex: testmplayer 20100606.log 3\nso the toleance is 3sec\n");
			exit(0);
	}
	strcpy(outputfile,argv[1]);
	sscanf(argv[2],"%d",&tolerance);
	if (argc==4)
	{
			sscanf(argv[3],"%d",&recordall);
	}
	
	if (search_file(vlistname, mntpath, vlistpath) == 0)
	{
			printf("can't find vlist.txt!\n You need to prepare vlist.txt. Thanks.\n");
			return 0;
	}
	printf("vlist is in %s\n", vlistpath );
	chdir(vlistpath);
        if(!setlocale(LC_CTYPE,""))
	{
		perror("set location fail(unicode)!");
		exit(EXIT_FAILURE);
	}

	if ((in=fopen(vlistname,"r"))==NULL) 
	{
		perror("File open error");
		exit(EXIT_FAILURE);
	}
	//start testing, but we need to create vresult.txt first
	if ((out=fopen(outputfile,"w"))==NULL) 
	{
		perror("File open error");
		fclose(in);
		exit(EXIT_FAILURE);
	}
	fprintf(out,"Let's RockNRoll!!\n");
	fprintf(out,"tolerance is %d second(s), record all is %d\n", tolerance, recordall);
	fprintf(out, "-----------------------------------------------------\n");
	fclose(out);
  
#ifndef DEBUG_ON_PC  
  // set resolution
  {
   uint mode;
   int	fd;   
   mode = 0x0a;
   
					  fd = open("/dev/graphics/fb0", O_RDWR, 0);
           if (ioctl(fd, SKYFB_SET_MODE_ONLY, &mode) != 0) {
                      perror("Set Display Mode failed!");
                      fclose(in);
                      exit(EXIT_FAILURE);
           }
  }
#endif
	while(fgets(tmp,sizeof(tmp),in)!=0) /* read a record */
	{
			int	hh;
	    int	mm;
	    int ss;
	    int	diffsec,correctsec;
	    char videofilepath[1024], filewithpath[1024];
	    
	    // for utf-8,, C make a mess with it.
      if (tmp[0]=='#'||tmp[1]=='#'||tmp[2]=='#'||tmp[3]=='#'||tmp[4]=='#')
      {
      	 continue;
      }
      recordcnt++;
		  parse(tmp,",",arr,&fldcnt);    /* whack record into fields */
		
			printf("%3d:%s  time=%s\n",recordcnt,arr[0],arr[1]);
			// search this video file.
			if (search_file(arr[0], ".", videofilepath) == 0)
			{
					printf("can't find %s!\n", arr[0]);
					if ((out=fopen(outputfile,"a"))==NULL) 
	 				{
							perror("File open error");
							exit(EXIT_FAILURE);
	 				}
    			fprintf(out,"**Oops!! can't find %s, please check vlist.txt and video file.\n", arr[0]);
					sync();
					fclose(out);
					miss++;
					continue;
			}
			sprintf(filewithpath,"%s/%s",videofilepath, arr[0]);
			printf("the video file is %s\n", filewithpath);
			
			sscanf(arr[1],"%d:%d:%d",&hh,&mm,&ss);
			correctsec = ss + (60*mm) + (60*60*hh);  // translate them to second.
			printf("correctsec = %d\n", correctsec);
#ifdef DEBUG_ON_PC			
	    continue;		
#endif	    

			gettimeofday(&tv_start, 0);
			//here!! run mplayer with those test video chips.
			total++;
			pid = fork();
          switch (pid)
          {
            case -1:
              return 1;
              break;
            case 0:
            	 execl("/system/bin/mplayer","mplayer","-skydroid","-forcelock","-quiet","-subcp","cp950","-font","/system/fonts/DroidSansFallback.ttf",filewithpath,NULL);
            	 break;
            default:
              waitpid(pid, &status, 0);
              break;
          }
			
			gettimeofday(&tv_last, 0);
			//printf("2. tv_last = %d\n",tv_last.tv_sec);
			diffsec = tv_last.tv_sec - tv_start.tv_sec;
		  if (((diffsec > correctsec) && ((diffsec-correctsec)>tolerance))||
		      ((diffsec < correctsec) && ((correctsec-diffsec)>tolerance))||
		      recordall==1)   
		  {	
		  	  // stupid, but I am lazy to think another way.
		  	  if (((diffsec > correctsec) && ((diffsec-correctsec)>tolerance))||
		      ((diffsec < correctsec) && ((correctsec-diffsec)>tolerance)))
		      {
		      		fail++;
		      }else{
		      		pass++;
		      }
					//write log file	
					if ((out=fopen(outputfile,"a"))==NULL) 
					{
							perror("File open error");
							exit(EXIT_FAILURE);
					}

		  		fprintf(out,"%s,%d,%d\n",arr[0],correctsec,diffsec);
		  		sync();
		  		fclose(out);
		  }else{
		  	  pass++;
		  }
	}
	  //write the final log file	
	 if ((out=fopen(outputfile,"a"))==NULL) 
	 {
				perror("File open error");
				exit(EXIT_FAILURE);
	 }	
    fprintf(out,"-------------------------------------------------------------------------\n");
		fprintf(out,"total tested %d files, %d file pass, %d files failed, %d files missed.\n",total, pass, fail, miss);
		fprintf(out,"the miss file is not included in tested file, please check vlist.txt\n");
		sync();
		fclose(out);
		// whatever write again.
		if ((out=fopen(outputfile,"a"))==NULL) 
	 {
				perror("File open error");
				exit(EXIT_FAILURE);
	 }	
    fprintf(out,"-------------------------------------------------------------------------\n");
		sync();
		fclose(out);
    fclose(in);
    printf("---------------------------------------------------------------------------------------\n");
		printf(" Testing is finished, total tested %d files, %d file pass, %d files failed, %d files missed.\n",total, pass, fail, miss);
		printf(" the miss file is not included in tested file, please check vlist.txt\n");
    return 0;	
}
