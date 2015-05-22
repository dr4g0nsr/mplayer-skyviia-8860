



#include "libmpdemux/demuxer.h"
#define HAVE_AV_CONFIG_H  
#include "libavformat/internal.h"

#ifdef SKY_HTTP

#define DEBUG_SKYHTTP 0 /*leochang ck if release ==>0!*/
int sky_http_enable = 0;


#if (DEBUG_SKYHTTP==1)
#define skyhttp_printf printf
#else
#define skyhttp_printf 
#endif


/* strcuture */
/*
 * An apple http stream consists of a playlist with media segment files,
 * played sequentially. There may be several playlists with the same
 * video content, in different bandwidth variants, that are played in
 * parallel (preferrably only one bandwidth variant at a time). In this case,
 * the user supplied the url to a main playlist that only lists the variant
 * playlists.
 *
 * If the main playlist doesn't point at any variants, we still create
 * one anonymous toplevel variant for this, to maintain the structure.
 */

/*
live ==> can't seek but need reload

VOD  ==> can seek but no need reload

security support latter 
*/


enum KeyType {
    KEY_NONE,
    KEY_AES_128,
};

#define MAX_URL_SIZE  4096

struct variant_info 
{
    char bandwidth[20];
};

struct segment {
    int     duration;
    char    url[MAX_URL_SIZE];
    char 	key[MAX_URL_SIZE];
    enum    KeyType key_type;
    uint8_t iv[16];
};

/*
 * Each variant has its own demuxer. If it currently is active,
 * it has an open ByteIOContext too, and potentially an AVPacket
 * containing the next packet from this stream.
 */
 
struct variant {
	int bandwidth;
	char url[MAX_URL_SIZE];      /* refine hrer */
	int index;
	int stream_offset;	
	int finished;
	int target_duration;
	int start_seq_no;
	int cur_seq_no;
	int n_segments;
	struct segment **segments;
	int needed, cur_needed;
	int64_t last_load_time;
	char key_url[MAX_URL_SIZE];
	uint8_t key[16];
};

typedef struct SKYAppleHTTPContext {
    int n_variants;
    struct variant **variants;
    int cur_seq_no;
    int end_of_segment;
    int first_packet;

	/* leochang add*/
	int64_t total_duration;  
	URLContext *h;
	int retry;
	int64_t filesize;
	int64_t cur_read_filesize;
	int end_of_file;
	int cur_var;		                
	int seek_var;
	int seek_seg;
	/* leochang add*/	
} SKYAppleHTTPContext;


static void make_absolute_url(char *buf, int size, const char *base,
                              const char *rel)
{
    char *sep;
	//skyhttp_printf("%s:%s\n",__FILE__,__func__);

	/* If rel actually is an absolute url, just copy it */
    if (!base || strstr(rel, "://") || rel[0] == '/') {
        av_strlcpy(buf, rel, size);
        return;
    }
    if (base != buf)
        av_strlcpy(buf, base, size);
    /* Remove the file name from the base url */
    sep = strrchr(buf, '/');
    if (sep)
        sep[1] = '\0';
    else
        buf[0] = '\0';
    while (av_strstart(rel, "../", NULL) && sep) {
        /* Remove the path delimiter at the end */
        sep[0] = '\0';
        sep = strrchr(buf, '/');
        /* If the next directory name to pop off is "..", break here */
        if (!strcmp(sep ? &sep[1] : buf, "..")) {
            /* Readd the slash we just removed */
            av_strlcat(buf, "/", size);
            break;
        }
        /* Cut off the directory name */
        if (sep)
            sep[1] = '\0';
        else
            buf[0] = '\0';
        rel += 3;
    }
    av_strlcat(buf, rel, size);
}


static int read_chomp_line(ByteIOContext *s, char *buf, int maxlen)
{
    int len = ff_get_line(s, buf, maxlen);
	//skyhttp_printf("%s:%s\n",__FILE__,__func__);
    while (len > 0 && isspace(buf[len - 1]))
        buf[--len] = '\0';
    return len;
}

static void free_segment_list(struct variant *var)
{
    int i;
    skyhttp_printf("%s:%s\n",__FILE__,__func__);

    for (i = 0; i < var->n_segments; i++)
        av_free(var->segments[i]);
    av_freep(&var->segments);
    var->n_segments = 0;
}


static void free_variant_list(SKYAppleHTTPContext *c)
{
    int i;
    skyhttp_printf("%s:%s\n",__FILE__,__func__);
    for (i = 0; i < c->n_variants; i++) {
        struct variant *var = c->variants[i];
        free_segment_list(var);

        av_free(var);
    }
    av_freep(&c->variants);
    c->n_variants = 0;
}

static struct variant *new_variant(SKYAppleHTTPContext *c, int bandwidth,
                                   const char *url, const char *base)
{
    struct variant *var = av_mallocz(sizeof(struct variant));
	skyhttp_printf("%s:%s\n",__FILE__,__func__);
	if (!var)
        return NULL;
    var->bandwidth = bandwidth;
    make_absolute_url(var->url, sizeof(var->url), base, url);
    dynarray_add(&c->variants, &c->n_variants, var);
    return var;
}

static int applehttp_probe(stream_t *p)
{
	skyhttp_printf("applehttp_probe\n");//leo del//
    /* Require #EXTM3U at the start, and either one of the ones below
     * somewhere for a proper match. */
    if (strncmp(p->buffer, "#EXTM3U", 7))
        return 0;
    if (strstr(p->buffer, "#EXT-X-STREAM-INF:")     ||
        strstr(p->buffer, "#EXT-X-TARGETDURATION:") ||
        strstr(p->buffer, "#EXT-X-MEDIA-SEQUENCE:"))
        return 100/*AVPROBE_SCORE_MAX*/;
    return 0;
}


static void handle_variant_args(struct variant_info *info, const char *key,
                                int key_len, char **dest, int *dest_len)
{
    if (!strncmp(key, "BANDWIDTH=", key_len)) {
        *dest     =        info->bandwidth;
        *dest_len = sizeof(info->bandwidth);
    }
}


struct key_info {
     char uri[MAX_URL_SIZE];
     char method[10];
     char iv[35];
};

static void handle_key_args(struct key_info *info, const char *key,
                            int key_len, char **dest, int *dest_len)
{
    if (!strncmp(key, "METHOD=", key_len)) {
        *dest     =        info->method;
        *dest_len = sizeof(info->method);
    } else if (!strncmp(key, "URI=", key_len)) {
        *dest     =        info->uri;
        *dest_len = sizeof(info->uri);
    } else if (!strncmp(key, "IV=", key_len)) {
        *dest     =        info->iv;
        *dest_len = sizeof(info->iv);
    }
}

static int parse_playlist(SKYAppleHTTPContext *c, const char *url,
                          struct variant *var, ByteIOContext *in)
{
    int ret = 0, duration = 0, is_segment = 0, is_variant = 0, bandwidth = 0;
    enum KeyType key_type = KEY_NONE;
    uint8_t iv[16] = "";
    int has_iv = 0;
    char key[MAX_URL_SIZE];
    char line[1024];
    const char *ptr;
    int close_in = 0;
	char *nurl_ptr;

	skyhttp_printf("parse_playlist\n");
		
    if (!in) {
        close_in = 1;
        nurl_ptr = strstr(url, "http");
        skyhttp_printf("try  url_fopen, nurl_ptr=%s\n", nurl_ptr);
        if ((ret = url_fopen(&in, nurl_ptr, URL_RDONLY)) < 0)
        {
            ret = -1;
            printf("url_fopen error!\n");
            ///close_in=0;
            goto fail;
        }
    }
		
    read_chomp_line(in, line, sizeof(line));
    skyhttp_printf("==> line=%s\n", line);
    if (strcmp(line, "#EXTM3U")) {
        ret = -1;
        goto fail;
    }

    if (var){
        free_segment_list(var);
        var->finished = 0;
    }

    while (!url_feof(in)) {
        read_chomp_line(in, line, sizeof(line));
		//skyhttp_printf("==> line=%s!\n", line);	
        if (av_strstart(line, "#EXT-X-STREAM-INF:", &ptr)) {
            struct variant_info info = {{0}};
            is_variant = 1;
            ff_parse_key_value(ptr, (ff_parse_key_val_cb) handle_variant_args,
                               &info);
            bandwidth = atoi(info.bandwidth);
/*
		} else if (av_strstart(line, "#EXT-X-KEY:", &ptr)) {
			struct key_info info = {{0}};
			ff_parse_key_value(ptr, (ff_parse_key_val_cb) handle_key_args,&info);
			key_type = KEY_NONE;
			has_iv = 0;
			if (!strcmp(info.method, "AES-128"))
				key_type = KEY_AES_128;
			if (!strncmp(info.iv, "0x", 2) || !strncmp(info.iv, "0X", 2)) {
				//leo//ff_hex_to_data(iv, info.iv + 2);
				has_iv = 1;
			}
			av_strlcpy(key, info.uri, sizeof(key));
*/
		} else if (av_strstart(line, "#EXT-X-TARGETDURATION:", &ptr)) {
			if (!var) {
				var = new_variant(c, 0, url, NULL);
				if (!var) {
					ret = -1;//AVERROR(ENOMEM);
					goto fail;
				}
			}
			var->target_duration = atoi(ptr);
        } else if (av_strstart(line, "#EXT-X-MEDIA-SEQUENCE:", &ptr)) {
            if (!var) {
				skyhttp_printf("v1 url:%s\n",url);//leo del
                var = new_variant(c, 0, url, NULL);
                if (!var) {
                    ret = -1;//AVERROR(ENOMEM);//leo
                    goto fail;
                }
            }
            var->start_seq_no = atoi(ptr);
        } else if (av_strstart(line, "#EXT-X-ENDLIST", &ptr)) {
            if (var)
                var->finished = 1;
            //printf("var->finished:%d\n",var->finished);
        } else if (av_strstart(line, "#EXTINF:", &ptr)) {
            is_segment = 1;
            duration   = atoi(ptr);
        } else if (av_strstart(line, "#", NULL)) {
            continue;
        } else if (line[0]) {
            if (is_variant) {
                if (!new_variant(c, bandwidth, line, url)) {
                    ret = -1;//AVERROR(ENOMEM);//leo
                    goto fail;
                }
                is_variant = 0;
                bandwidth  = 0;
            }
            if (is_segment) {
                struct segment *seg;
                if (!var) {
                    var = new_variant(c, 0, url, NULL);
                    if (!var) {
                        ret = -1;//AVERROR(ENOMEM);//leo 
                        goto fail;
                    }
                }
                seg = av_malloc(sizeof(struct segment));
                if (!seg) {
                    ret = -1;/*AVERROR(ENOMEM)*///leo 
                    goto fail;
                }
                seg->duration = duration;
                //seg->key_type = key_type;
                //if (has_iv) {
                //    memcpy(seg->iv, iv, sizeof(iv));
                //} else {
                //    //int seq = var->start_seq_no + var->n_segments;
                //    //memset(seg->iv, 0, sizeof(seg->iv));
                //    //leo//AV_WB32(seg->iv + 12, seq);
                //}
                //make_absolute_url(seg->key, sizeof(seg->key), url, key);				
                make_absolute_url(seg->url, sizeof(seg->url), url, line);
                dynarray_add(&var->segments, &var->n_segments, seg);
                is_segment = 0;
            }
        }
    }
    
    if (var)
    {
        var->last_load_time = av_gettime();
        skyhttp_printf("var->last_load_time:%x\n",var->last_load_time);
    }

fail:
    if (close_in)
    {
        if(in)
        url_fclose(in);
    }
    return ret;
}

static int applehttp_read_header(stream_t *s)
{
    SKYAppleHTTPContext *c = s->priv;
    int ret = 0, i;//, stream_offset = 0;
    char *nurl_ptr;
    int url_open_err=0;
    int fd=0;
    skyhttp_printf("read_header\n");
    skyhttp_printf("s->url:%s\n",s->url);
		
	nurl_ptr = strstr(s->url, "http");
		
	skyhttp_printf("new nurl_ptr=%s\n", nurl_ptr);

	if ((ret = parse_playlist(c, nurl_ptr, NULL, NULL)) < 0)
		goto fail;
    		
	if (c->n_variants == 0) {
		//av_log(NULL, AV_LOG_WARNING, "Empty playlist\n");
		printf("Empty playlist\n");
		//ret = -1;//AVERROR_EOF;
		goto fail;
	}

	skyhttp_printf("c->n_variants:%d\n",c->n_variants);
	skyhttp_printf("c->variants[0]->n_segments:%d\n",c->variants[0]->n_segments);

    /* If the playlist only contained variants, parse each individual
     * variant playlist. */
    if (c->n_variants > 1 || c->variants[0]->n_segments == 0) {
		skyhttp_printf("If the playlist only contained variants, parse each individual\n");

        for (i = 0; i < c->n_variants; i++) {
            struct variant *v = c->variants[i];
            nurl_ptr = strstr(v->url, "http");
			skyhttp_printf("nurl_ptr:%s\n",nurl_ptr);						
            if ((ret = parse_playlist(c, nurl_ptr, v, NULL)) < 0)
            {
                goto fail;
            }
        }
    }

    if (c->variants[0]->n_segments == 0) {
        printf(" bug Empty playlist\n");
        //ret = -1//AVERROR_EOF;
        goto fail;
    }


	/* If this isn't a live stream, calculate the total duration of the stream. */
	if (c->variants[0]->finished) {
		int64_t duration = 0;
		skyhttp_printf("VOD stream!\n");
		for (i = 0; i < c->variants[0]->n_segments; i++)
			duration += c->variants[0]->segments[i]->duration;
		c->total_duration = duration * AV_TIME_BASE;
        skyhttp_printf("c->total_duration:%x\n",c->total_duration);
	}
  	skyhttp_printf("c->n_variants:%d\n",c->n_variants);
	skyhttp_printf("c->variants[0]->n_segments:%d\n",c->variants[0]->n_segments);


    /*leo test only decode first varient for multiple bandwidth*/     
    for (i = 0; i <c->n_variants;i++)
    {
      if(i==0)
        c->variants[i]->needed=1;
      else
        c->variants[i]->needed=0;
    }

  
    /* Open the demuxer for each variant */
    for (i = 0; i < c->n_variants; i++) 
	{
        struct variant *v = c->variants[i];
        if (v->n_segments == 0)
            continue;
		
 
        if(c->variants[i]->needed==0)
          continue;


		/* If this is a live stream with more than 3 segments, start at the
		* third last segment. */
		v->cur_seq_no = v->start_seq_no;
		if (!v->finished && v->n_segments > 3)
			v->cur_seq_no = v->start_seq_no + v->n_segments - 3;

        skyhttp_printf("v->cur_seq_no:%d\n",v->cur_seq_no);
        skyhttp_printf("v->start_seq_no:%d\n",v->start_seq_no);  
        skyhttp_printf("v->n_segments:%d\n",v->n_segments);    

        //v->cur_seq_no=v->start_seq_no+v->n_segments-4;
        skyhttp_printf("v->cur_seq_no:%d\n",v->cur_seq_no);       

        c->cur_seq_no = v->cur_seq_no;

		nurl_ptr = strstr(v->segments[v->cur_seq_no - v->start_seq_no]->url, "http");
		skyhttp_printf("v->n_segments:%d\n",v->n_segments);
		skyhttp_printf("nurl_ptr|x|:%s\n",nurl_ptr);

		url_open_err = url_open(&c->h, nurl_ptr, URL_RDONLY);
		if (url_open_err < 0)
		{
			printf("url_open_err:%d\n",url_open_err);
			goto fail;
		} 

		c->filesize=url_filesize(c->h);
		/*todo add ck <0 ?*/		
		//HTTPContext *s = h->priv_data;
		skyhttp_printf("filesize:0x%x\n",c->filesize);
		/* cur_var cur_*/
		c->cur_var=i;
		ret=0;//ok
		return ret;
    }

fail:
	printf("%s fail:\n",__func__);
	if(c)
    	free_variant_list(c);
	ret=-1;//error!
    return ret;
}


/*to ck this api's return value!*/
/*if error we retuen -1 */
#define SKYHTTP_FILL_ERROR -1
#define SKYHTTP_FILL_NONE 0

static int skyhttp_get_new_segment(SKYAppleHTTPContext *c)
{
	int i=0;
	struct variant *v = c->variants[c->cur_var];
	int cur_var;

	skyhttp_printf("skyhttp_get_new_segment\n");

	if (v->cur_seq_no >= v->start_seq_no + v->n_segments) 
	{
		c->cur_var++;

		if(c->cur_var>=c->n_variants)
		{
			return SKYHTTP_FILL_ERROR;			
		}
		else
		{
			for (i = c->cur_var; i < c->n_variants; i++) 
			{
                struct variant *v_new = c->variants[i];
                if(v_new->n_segments==0)
                    continue;
   
                if(c->variants[i]->needed==0)
                    continue;
  
                v_new->cur_seq_no = v_new->start_seq_no;
                c->cur_seq_no     = v_new->cur_seq_no;
                c->cur_var =i;
                skyhttp_printf("c->cur_var:%d\n",c->cur_var);
                return SKYHTTP_FILL_NONE;
			}
			return SKYHTTP_FILL_ERROR;
		}	

		/*update new total duration*/
		/* If this isn't a live stream, calculate the total duration of the stream. */
#if 0
		cur_var=c->cur_var;
		if (c->variants[cur_var]->finished) {
			int64_t duration = 0;
			skyhttp_printf("VOD stream!\n");
			for (i = 0; i < c->variants[cur_var]->n_segments; i++)
				duration += c->variants[cur_var]->segments[i]->duration;
			c->total_duration = duration * AV_TIME_BASE;
			skyhttp_printf("c->total_duration:%x\n",c->total_duration);
		}
#endif	
	}
	else
	{
		skyhttp_printf("@@@@@@@	new segment: v->cur_seq_no:%d@@@@@@@@\n\n",v->cur_seq_no);
	}
}

static int skyhttp_reconnect(SKYAppleHTTPContext *c)
{
	char *nurl_ptr;
	struct variant *v ;
	int ret;
    if(c->h)
	    url_close(c->h);                  /*this api already free h!!*/
	c->h = NULL;

	v = c->variants[c->cur_var];
	/* should we ck v->segments[cur_seg] == NULL or not?*/	
		
	skyhttp_printf("\n\n\n\n\n\n !@#$%^&*(=========skyhttp_reconnect v->cur_seq_no  :%d start_seq_no:%d================\n\n",v->cur_seq_no,v->start_seq_no); 

	nurl_ptr = strstr(v->segments[v->cur_seq_no - v->start_seq_no]->url, "http");
	skyhttp_printf("skyhttp_reconnect nurl_ptr|x|:%s\n",nurl_ptr);		
	ret = url_open(&c->h, nurl_ptr,URL_RDONLY); 	
	if (ret < 0)
	{ 
		printf("skyhttp reconnect url_open error ret:%d\n",ret);
		goto skyhttp_reconnect_fail;
	}
		

	ret = url_seek(c->h,c->cur_read_filesize,SEEK_CUR);

    if (ret < 0)
    { 
        int64_t size,seeklen=c->cur_read_filesize;
        while(seeklen > 0){
            char buf[1024];
            size = (seeklen < 1024) ? seeklen : 1024;
            size = url_read(c->h, buf, size);
            skyhttp_printf("seeklen:%d size   :%d\n",seeklen,size);

            if(size <= 0){
                //printf("skip failed\n");
                //printf("skyhttp url_seekt error:%d\n",ret);
                goto skyhttp_reconnect_fail;
            }
            seeklen -= size;
        }
    }
    return 0;

skyhttp_reconnect_fail:
    if(c->h)
        url_close(c->h);                  /*this api already free h!!*/
	c->h = NULL;
	return SKYHTTP_FILL_ERROR;
}




static int skyhttp_fill_buffer(stream_t *s, char* buffer, int max_len)
{
	/*in this api if error==> return 0, if error return bytes*/
	int url_read_bytes,ret;
	SKYAppleHTTPContext *c = s->priv;
	char *nurl_ptr;
	struct variant *v ;

	/* may add end of file check*/
	if(c->end_of_file)
	{
		printf("stream enf of file!\n");
		return SKYHTTP_FILL_ERROR;
	}

	v = c->variants[c->cur_var];

	if(!c->h)
	{
reload:
		/* If this is a live stream and target_duration has elapsed since
		* the last playlist reload, reload the variant playlists now. */
		if (!v->finished)
		{
			if(av_gettime() - v->last_load_time >= v->target_duration*1000000 )
			{
				skyhttp_printf("a live stream and target_duration:%d has elapsed!\n",v->target_duration*1000000);
				if((ret = parse_playlist(c, v->url, v, NULL)) < 0)
				{
					skyhttp_printf("parse_playlist err ret:%d\n",ret);
					return SKYHTTP_FILL_ERROR; //0 for stop
				}
			}
		}

		if (v->cur_seq_no < v->start_seq_no) {
			skyhttp_printf("skipping %d segments ahead, expired from playlists\n",v->start_seq_no - v->cur_seq_no);
			v->cur_seq_no = v->start_seq_no;
		}

		/**/
		if(v->finished)
		{
			ret = skyhttp_get_new_segment(c);
			if(ret<0)
			{
				printf("skyhttp_get_new_segment:%d\n",ret);
				return SKYHTTP_FILL_ERROR; //-1 for stop
			}
		}
		else
		{
			//skyhttp_printf("live is not tested now!!\n");
			if (v->cur_seq_no >= v->start_seq_no + v->n_segments) 
			{
				//skyhttp_printf("a live streaming and wait new stream v->cur_seq_no:%d\n",v->cur_seq_no);
				//skyhttp_printf("v->start_seq_no:%d v->n_segments:%d\n",v->start_seq_no,v->n_segments);
                int prevent_hang=0;
				 //not tested code
				while (av_gettime() - v->last_load_time < v->target_duration*1000000) 
				{
					if (url_interrupt_cb()||(prevent_hang>1000))
					{
                        printf("bef usleep v->target_duration:0x%x\n v->last_load_time:0x%x\n av_gettime():0x%x\n",v->target_duration,v->last_load_time,av_gettime());
                        printf("live reload error:prevent:%d\n",prevent_hang);
						return 0;
					}
					usleep(100*1000);
                    prevent_hang++;
				}
				
				/* Enough time has elapsed since the last reload */
				/* leo todo avoid forever loop*/
				/* add retry counter*/
                skyhttp_printf("v->target_duration:0x%x\n v->last_load_time:0x%x\n av_gettime():0x%x\n",v->target_duration,v->last_load_time,av_gettime());
				skyhttp_printf("a live streaming wait new stream prevent_hang:%d\n",prevent_hang);		
				goto reload;
			}
		}
		
		v = c->variants[c->cur_var];
		/* should we ck v->segments[cur_seg] == NULL or not?*/		
		skyhttp_printf("v->cur_seq_no	 :%d start_seq_no:%d\n",v->cur_seq_no,v->start_seq_no);	
        nurl_ptr = strstr(v->segments[v->cur_seq_no - v->start_seq_no]->url, "http");
		skyhttp_printf("nurl_ptr|x|:%s\n",nurl_ptr);		
		ret = url_open(&c->h,	nurl_ptr,URL_RDONLY);		
		if (ret < 0)
		{ 
			skyhttp_printf("url_open error ret:%d\n",ret);
			return SKYHTTP_FILL_ERROR;
		}
		c->filesize=url_filesize(c->h);
		skyhttp_printf("c->filesize:0x%x\n",c->filesize);
		/*add ck c->filesize<=0 lleo*/
		if(c->filesize<=0)
			return SKYHTTP_FILL_ERROR;
	}/*end of 	if(!c->h)*/


url_read_again :
	url_read_bytes=url_read(c->h,buffer,max_len);
   
	if((url_read_bytes<=0)&&(c->cur_read_filesize<c->filesize))
	{
		usleep(100000);
		skyhttp_printf("@@@ r:0x%x ml:0x%x a:0x%x ",url_read_bytes,max_len,c->cur_read_filesize);
		skyhttp_printf("fs:0x%x\n",c->filesize);
		c->retry++;
		if(c->retry>3)
		{
			c->retry=0;
			/* reconnect */
			ret=skyhttp_reconnect(c);
			if(ret<0)
			return SKYHTTP_FILL_ERROR; /* read error return*/
		}
		goto url_read_again;
	}
	else
	{
		/* reset retry counter*/
		c->retry=0;
	}

	c->cur_read_filesize+=url_read_bytes;

	//3.open new url
	if(c->cur_read_filesize>=c->filesize)
	{
		skyhttp_printf("n\@@@@@@ ###  now we need change url! ### @@@@@@@@@@@\n"); 			
        if(c->h)
            url_close(c->h);/*this api already free h!!*/
		c->h = NULL;
		c->cur_read_filesize=0;			
		c->end_of_segment = 0;
		v->cur_seq_no++;
		c->cur_seq_no = v->cur_seq_no;
        skyhttp_printf("v->cur_seq_no:%d\n",v->cur_seq_no);
		/*choose new and return*/
	}

	return url_read_bytes;
}


double sky_http_pts=0 ;



static int skyhttp_seek(stream_t *stream, off_t newpos) 
{
	SKYAppleHTTPContext* c=stream->priv;
	int ret, i,j,cur_var;
	off_t timestamp = (uint64_t)sky_http_pts;//mypts/1000000;
	char *nurl_ptr;
	struct variant *v ;

	if(!c->variants[c->cur_var]->finished)
    {
        printf("skyhttp_seek  return -1\n");
        return -1;//AVERROR(ENOSYS);
    }
	
	ret = -1;
	for (i = 0; i < c->n_variants; i++) {
		/* Reset reading */
		struct variant *var = c->variants[i];
		//int64_t pos = 0;
		int pos = 0;

        if(var->needed==0)
          continue;


		/* Locate the segment that contains the target timestamp */
		for (j = 0; j < var->n_segments; j++) {
            if (timestamp >= pos &&
				timestamp < pos + var->segments[j]->duration) 
			{

                //c->seek_seg = var->start_seq_no + j;
                c->cur_var =i;

				ret = 1;

				/* find new var and cur_seq_no and error handle*/
				/* by seek_var seek_seq*/
                if(c->h)
                    url_close(c->h);/*this api already free h!!*/
				c->h = NULL;
				c->cur_read_filesize=0;         
				c->end_of_segment = 0;

                //c->cur_var = c->seek_var;
                v = c->variants[c->cur_var];
                //c->cur_seq_no = v->cur_seq_no =c->seek_seg;
                c->cur_seq_no = v->cur_seq_no =var->start_seq_no + j;
                    
                /* should we ck v->segments[cur_seg] == NULL or not?*/      
                nurl_ptr = strstr(v->segments[v->cur_seq_no-v->start_seq_no]->url, "http");
                    
                skyhttp_printf("seek change nurl_ptr|x|:%s\n",nurl_ptr);        
                ret = url_open(&c->h, nurl_ptr,URL_RDONLY);     
                if (ret < 0)
                { 
                    skyhttp_printf("url_open error ret:%d\n",ret);
                    return SKYHTTP_FILL_ERROR;
                }
                c->filesize=url_filesize(c->h);
                skyhttp_printf("c->filesize:0x%x\n",c->filesize);
                /*add ck c->filesize<=0 lleo*/
                if(c->filesize<=0)
                {
                    ret=-1;
                    goto end;
                }
				ret=1;
				break;
            }
        	pos += var->segments[j]->duration;
		}/*end of for j*/
	}/*end of for i*/
end:

	if(ret==-1)
	{
		skyhttp_printf("seek to end of file!\n");
        if(c->h)
		    url_close(c->h);/*this api already free h!!*/
		c->h = NULL;
		c->end_of_file=1;
	}
	return ret;
}

static int skyhttp_control(stream_t *stream, int cmd, void* arg) 
{
    SKYAppleHTTPContext* c=stream->priv;
	switch(cmd)
    {
        case STREAM_CTRL_GET_TIME_LENGTH:
            //printf("STREAM_CTRL_GET_TIME_LENGTH\n");
            *((double *)arg) = (double)(c->total_duration/AV_TIME_BASE);
            //printf("c->total_duration/AV_TIME_BASE:%d\n",c->total_duration/AV_TIME_BASE);
            return 1;
            break;

		case STREAM_CTRL_SEEK_TO_TIME:
            //printf("STREAM_CTRL_SEEK_TO_TIME\n");
            //uint64_t tm = (uint64_t) (*((double*)arg) * AV_TIME_BASE);
			////mypts = *(double*)arg;
			//printf("mypts:%f\n",sky_http_pts);
            //skyhttp_seek(stream,tm);//todo add error handle//
            //stream_seek
            //return 1;
            return -1;
			break;
		
        default :
            //printf("cmd:%x\n",cmd);
            break;
	}
	return -1;
}


static void skyhttp_close(stream_t *stream)
{
	SKYAppleHTTPContext *c = stream->priv;
	skyhttp_printf("~~~~skyhttp_close~~~~\n\n");
	free_variant_list(c);/* free varients and segments*/

	if(c->h)
		url_close(c->h); 	/* close url*/
	c->h=NULL;

	if(stream->priv)
		av_free(stream->priv);
	
	stream->priv=NULL;
}

static int skyhttp_open(stream_t *stream, int mode, void *opts, int *file_format)
{
	int ret;
	skyhttp_printf("%s: @@@@@ sky_http_enable:%d\n", __func__,sky_http_enable);

	if(sky_http_enable==0)
	{
		printf("skyhttp_open :: sky_http_enable==0\n");
		return	STREAM_UNSUPPORTED;
	}
	
	//struct stream_priv_s* p = (struct stream_priv_s*)opts;
	//URL_t *url = url_new(stream->url);
	skyhttp_printf("%s:\n", __func__);
	skyhttp_printf("stream->url :%s\n",stream->url);
	skyhttp_printf("*file_format:%d\n",*file_format);
	skyhttp_printf("*mode       :%d\n",mode);

	av_register_all();
	
	stream->priv=av_mallocz(sizeof(SKYAppleHTTPContext));
	
	if(!stream->priv)
	{
		printf("allocate apple_http_ctx fail!\n");
		return STREAM_ERROR;
	}
	
	ret = applehttp_read_header(stream);
	if(ret<0)
	{
		if(stream->priv)
			av_free(stream->priv);
			stream->priv=NULL;
			return	STREAM_UNSUPPORTED;
	}
	stream->fill_buffer = skyhttp_fill_buffer;
	stream->close       = skyhttp_close;
	stream->seek        = skyhttp_seek;
	stream->control     = skyhttp_control;
	//add return stream type
	////*file_format = DEMUXER_TYPE_MPEG_TS;
    stream->type = STREAMTYPE_STREAM;
	stream->fd = -1;
 	skyhttp_printf("stream->fd:%d\n",stream->fd);
	return	STREAM_OK;
}


const stream_info_t stream_info_skyhttp =
{
	"Skyviia applehttp streaming",
	"skyhttp",
	"Leo Chang",
	"provide an interface to apple http live stream",
	skyhttp_open,
	{ "http", NULL },
	NULL,
	1 // Urls are an option string
};

#endif 


