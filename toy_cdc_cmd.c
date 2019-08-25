#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <libserialport.h>

#define VENDOR_ID	0x04d8
#define PRODUCT_ID	0xfd08

struct sp_port * port = NULL;
int trans_count = 0;
unsigned char trans_data[4092+1];	// 62x66
int varbose = 0;

void dump(char cType,const char* data,int size);
void usage();
int toy_puts(char * data, int size);
int toy_flush();

int toy_open()
{
	int idx = 0;
	struct sp_port **ports;
	struct sp_port *port0 = NULL;
	enum sp_return ret;
	int i;
	int cnt = -1;

	ret = sp_list_ports(&ports);
	if(ret != SP_OK)
	{
		fprintf(stderr,"ERROR:%d = sp_list_ports() %d %s\n", ret, sp_last_error_code(), sp_last_error_message());
		return -1;
	}

	for (i = 0; ports[i]; i++)
	{
		int vid,pid;
		ret = sp_get_port_usb_vid_pid (ports[i], &vid, &pid);
		if(!ret && vid == VENDOR_ID && pid == PRODUCT_ID)
		{
			cnt++;
			if(cnt == idx)
			{
				sp_get_port_by_name(sp_get_port_name(ports[i]), &port0);
				break;
			}
		}
	}
	sp_free_port_list(ports);
	if(!port0)
	{
		fprintf(stderr,"ERROR:Can't find serial port. vid:%04X pid:%04X\n", VENDOR_ID, PRODUCT_ID);
		return -1;
	}

	ret = sp_open(port0,SP_MODE_READ_WRITE);
	if(ret != SP_OK)
	{
		fprintf(stderr,"ERROR:%d = sp_open() %d %s\n", ret, sp_last_error_code(), sp_last_error_message());
		sp_free_port(port0);
		return -1;
	}

	struct sp_port_config * config = NULL;
	ret = sp_new_config(&config);
	if(ret != SP_OK)
	{
		fprintf(stderr,"ERROR:%d = sp_new_config() %d %s\n", ret, sp_last_error_code(), sp_last_error_message());
	}
	sp_get_config(port0,config);
	sp_set_config_baudrate(config,115200);
	sp_set_config_bits(config,8);
	sp_set_config_parity(config,SP_PARITY_NONE);
	//sp_set_config_rts(config,SP_RTS_ON);
	//sp_set_config_cts(config,SP_CTS_IGNORE);
	//sp_set_config_dtr(config,SP_DTR_ON);
	sp_set_config_xon_xoff(config,SP_XONXOFF_DISABLED);
	ret = sp_set_config(port0,config);
	if(ret != SP_OK)
	{
		fprintf(stderr,"ERROR:%d = sp_set_config() %d %s\n", ret, sp_last_error_code(), sp_last_error_message());
	}
	sp_free_config(config);

	port = port0;

	toy_flush();

	return 0;
}

int toy_close()
{
	enum sp_return ret;

	usleep(10 * 1000);		// 10mS Is this correct at this time?

	for(int i = 0;i < 5;i++)
	{
		int ret;
		ret = toy_puts("\0",1);
		if(ret != 1)
		{
			fprintf(stderr,"ERROR:No Responce from IrToy.99 %d\n",i);
			return -1;
		}
		usleep(10 * 1000);		// 10mS Is this correct at this time?
	}

	ret = sp_close(port);
	if(ret != SP_OK)
	{
		return -1;
	}
	sp_free_port(port);
	port = NULL;

	return 0;
}

int toy_gets(char * data, int size)
{
	int mS = 100;
	int res = 0;
	int i,ret,count;
	char * poi;
	size_t remain;

	memset(data,0,size);

	count = 0;
	poi = data + count;
	remain = size - count;
	for(i = 0;;i++)
	{
		if(remain <= 0)
		{
			break;
		}
		if(strstr((const char*)data,"\r\n") != NULL)
		{
			break;
		}
		ret = sp_blocking_read_next(port,poi, remain, mS);
		if(ret > 0)
		{
			count += ret;
			poi = data + count;
			remain = size - count;
			res = count;
		}
		else if(ret == 0)
		{
			if(i != 0)
			{
				break;
			}
		}
		else //if(ret == 0)
		{
			res = ret;
			break;
		}
	}
	if(res >= 0)
	{
		if(varbose)
		{
			dump('R',data,res);
		}
	}
	return res;
}

int toy_puts(char * data, int size)
{
	enum sp_return ret;
	ret = sp_blocking_write(port, data, size,1000);
	if(ret != size)
	{
	  return -1;
	}
	if(varbose)
	{
		dump('S',data,size);
	}
	return ret;
}

int toy_flush()
{
	enum sp_return ret;
	ret = sp_flush(port,SP_BUF_BOTH);
	if(ret != SP_OK)
	{
		return -1;
	}
	return 0;
}

int toy_cmd_res(const char* cmd1,char* data,int size)
{
	int result = -1;
	int ret;

	ret = toy_puts((char *)cmd1, strlen(cmd1));
	if(ret < 0)
	{
		goto EXIT_PATH;
	}

	ret = toy_gets(data,size);
	if(ret <= 0)
	{
		fprintf(stderr,"No response form IrToy.5\n");
		goto EXIT_PATH;
	}
	result = ret;
EXIT_PATH:
	return result;
}

int toy_reset()
{
	int ret;
	char buf[32];

	for(int i = 0;i < 5;i++)
	{
		ret = toy_puts("\0",1);
		if(ret != 1)
		{
			fprintf(stderr,"ERROR:No Responce from IrToy.1 %d\n",i);
			return -1;
		}
		usleep(10 * 1000);		// 10mS Is this correct at this time?
	}

	memset(buf,0,sizeof(buf));
	ret = toy_cmd_res("v",buf,sizeof(buf));
	if(ret < 4)
	{
		fprintf(stderr,"ERROR:Initialization failed IrToy.\n");
		return -1;
	}
	memset(buf,0,sizeof(buf));
	ret = toy_cmd_res("S",buf,sizeof(buf));
	if(ret != 3 && strcmp(buf,"S01"))
	{
		fprintf(stderr,"ERROR:Initialization failed IrToy.\n");
		return -1;
	}
	usleep(10 * 1000);		// 10mS Is this correct at this time?
	ret = toy_puts("\x24",1);
	if(ret != 1)
	{
		fprintf(stderr,"ERROR:No Responce from IrToy.2\n");
		return -1;
	}
	usleep(10 * 1000);		// 10mS Is this correct at this time?
	ret = toy_puts("\x25",1);
	if(ret != 1)
	{
		fprintf(stderr,"ERROR:No Responce from IrToy.3\n");
		return -1;
	}
	usleep(10 * 1000);		// 10mS Is this correct at this time?
	ret = toy_puts("\x26",1);
	if(ret != 1)
	{
		fprintf(stderr,"ERROR:No Responce from IrToy.4\n");
		return -1;
	}
	usleep(10 * 1000);		// 10mS Is this correct at this time?
	return 0;
}

int toy_bcmd_res(char * cmd1, int sz,
				 char * res1, int* psz1)
{
	int result = -1;
	int ret;

	ret = toy_puts((char*)cmd1, sz);
	if(ret < 0)
	{
		goto EXIT_PATH;
	}
	ret = toy_gets(res1,*psz1);
	if(ret <= 0)
	{
		*psz1 = 0;
		fprintf(stderr,"No response form IrToy.6\n");
		goto EXIT_PATH;
	}
	*psz1 = ret;
	result = 0;
EXIT_PATH:
	return result;
}

int hex2ary(const char* hex)
{
	int ret,len;
	unsigned int x;
	char c;
	char tmp[4];

	trans_count = 0;
	memset(trans_data,0,sizeof(trans_data));

	memset(tmp,0,sizeof(tmp));
	len = strlen(hex);
	for(int i = 0;i < (len + 1);i++)
	{
		if(isspace(*hex) ||
		   i == len)
		{
			if(strlen(tmp))
			{
				ret = sscanf(tmp,"%x%c",&x,&c);
				if(ret == 1)
				{
					trans_data[trans_count] = (unsigned char)x;
					trans_count++;
					if(trans_count >= sizeof(trans_data))
					{
						fprintf(stderr,"ERROR:Hex string is too long.\n");
						return -1;
					}
					memset(tmp,0,sizeof(tmp));
				}
				else
				{
					fprintf(stderr,"ERROR:Incorrect hex number. %s\n",tmp);
					return -1;
				}
			}
		}
		else
		{
			int len = strlen(tmp);
			tmp[len] = *hex;
			if(len >= 3)
			{
				fprintf(stderr,"ERROR:Hex digit is too long.\n");
				return -1;
			}
		}
		hex++;
	}
	if(trans_count >= 2)
	{
		if( trans_data[trans_count - 1] != 0xff ||
			trans_data[trans_count - 2] != 0xff)
		{
			if(trans_count <= (sizeof(trans_data) - 2))
			{
				trans_data[trans_count] = 0xff;
				trans_count++;
				trans_data[trans_count] = 0xff;
				trans_count++;
				fprintf(stderr,"waring:Adjust terminate 0xff,0xff.\n");
			}
		}
	}
	fprintf(stderr,"%d bytes\n",trans_count);
	return 0;
}

// int trans_count = 0;
// unsigned char trans_data[1024];
int toy_transfer(const char* hex)
{
	int res = -1;
	int ret;
	int ssz,rsz;
	char rbuf[32];

	ret = hex2ary(hex);
	if(ret)
	{
		return -1;
	}
	ret = toy_open();
	if(ret)
	{
		return -1;
	}
	ret = toy_flush();
	if(ret)
	{
		goto EXIT_PATH;
	}
	ret = toy_reset();
	if(ret)
	{
		goto EXIT_PATH;
	}

	fprintf(stderr, "Initialize complate.\n");

	rsz = 1;
	memset(rbuf,0,sizeof(rbuf));
	ret = toy_bcmd_res("\x3", 1,
		 rbuf,&rsz);
	if(ret || rsz != 1 || rbuf[0] != 0x3E)
	{
		goto EXIT_PATH;
	}

	for(int i = 0;i < trans_count;i+=62)
	{
		ssz = 62;
		if(i + ssz > trans_count)
		{
			ssz = (trans_count % 62);
		}
		rsz = 1;
		memset(rbuf,0,sizeof(rbuf));
		ret = toy_bcmd_res((char *)&trans_data[i],
			ssz,
			rbuf,&rsz);
		if(ret || rsz != 1 || rbuf[0] != 0x3E)
		{
			goto EXIT_PATH;
		}
	}
	memset(rbuf,0,sizeof(rbuf));
	ret = toy_gets(rbuf,sizeof(res));
	if(ret != 3 || rbuf[0] != 't' || rbuf[2] != 'X')
	{
		goto EXIT_PATH;
	}
	fprintf(stderr,"SUCCESS:Tansfer %d bytes\n",trans_count);
	res = 0;
EXIT_PATH:
	ret = toy_close();
	return res;
}

int toy_receive()
{
	int res = -1;
	int ret;
	char rbuf[4092];
	int off,rsz,tmout;

	ret = toy_open();
	if(ret)
	{
		return -1;
	}
	ret = toy_flush();
	if(ret)
	{
		goto EXIT_PATH;
	}
	ret = toy_reset();
	if(ret)
	{
		goto EXIT_PATH;
	}

	fprintf(stderr, "Initialize complate.\n");

	memset(rbuf,0,sizeof(rbuf));
	off = 0;
	rsz = sizeof(rbuf);
	tmout = 0;
	while(1)
	{
		ret = toy_gets(&rbuf[off],rsz);
		if(ret > 0)
		{
			off += ret;
			rsz -= ret;

			if( ((unsigned char *)rbuf)[off - 1] == 0xff &&
					((unsigned char *)rbuf)[off - 2] == 0xff)
			{
				fprintf(stderr, "SUCCESS:Receive complate. length=%d\n",off);
				res = 0;
				break;
			}
			if(rsz == 0)
			{
				fprintf(stderr, "ERROR:receive buffer overrun. length=%d\n",off);
				break;
			}
			usleep(10 * 1000);		// 10mS Is this correct at this time?
		}
		else if(ret == 0)
		{
			tmout ++;
			if(tmout >= (3000 / 200))
			{
				fprintf(stderr, "ERROR:receive timeout. length=%d\n",off);
				break;
			}
		}
		else if(ret < 0)
		{
			break;
		}
	}
	if((res == 0) && (off != 0))
	{
		for(int i = 0;i < off;i++)
		{
			printf("%02X ",((unsigned int)rbuf[i]) & 0xff);
		}
		printf("\n");
	}
EXIT_PATH:
	ret = toy_close();
	return res;
}

int main(int argc,char* argv[])
{
	int ret;
	char cmd = '\0';
	char* hex = NULL;

	int opt;
	while ((opt = getopt(argc, argv, "vrt:")) != -1)
	{
		switch (opt)
		{
			case 'r':
				cmd = opt;
				break;
			case 'v':
				varbose = 1;
				break;
			case 't':
				cmd = opt;
				hex = optarg;
				break;
		}
	}

	if(cmd == '\0')
	{
		usage();
		return 0;
	}

	switch(cmd)
	{
		case 'r':
			ret = toy_receive();
			break;
		case 't':
			ret = toy_transfer(hex);
			break;
	}

	return ret;
}

void usage()
{
  fprintf(stderr, "usage: toy_ir_cmd <option>\n");
  fprintf(stderr, "	 -r		  \tReceive Infrared code.\n");
  fprintf(stderr, "	 -t 'hex'\tTransfer Infrared code.\n");
  fprintf(stderr, "	 -t \"$(cat XXX.txt)\"\n");
  fprintf(stderr, "	 -t \"`cat XXX.txt`\"\n");
  fprintf(stderr, "	 -v		  \t varbose mode.\n");
}

void dump(char cType, const char* data,int size)
{
	char c;
	int i,j;
	for(i = 0;i < size;i+=16)
	{
		fprintf(stderr,"%c %04X ",cType,i);
		for(j = 0;j < 16;j++)
		{
			if((i + j) >= size)
			{
				break;
			}
			c = data[i + j];
			fprintf(stderr,"%02X ",(int)c & 0xFF);
			if(j == 7)
			{
			 fprintf(stderr,"- ");
			}
		}
		for(/*j = 0*/;j < 16;j++)
		{
			fprintf(stderr,"   ");
			if(j == 7)
			{
				fprintf(stderr,"- ");
			}
		}
		for(j = 0;j < 16;j++)
		{
			if((i + j) >= size)
			{
				break;
			}
			c = data[i + j];
			fprintf(stderr,"%c",isprint(c) ? c : '?');
			if(j == 7)
			{
				fprintf(stderr," ");
			}
		}
		for(/*j = 0*/;j < 16;j++)
		{
			fprintf(stderr," ");
			if(j == 7)
			{
				fprintf(stderr," ");
			}
		}
		fprintf(stderr,"\n");
	}
}
