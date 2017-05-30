
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <linux/msdos_fs.h>

#define SECTORSIZE 512   //bytes
#define BLOCKSIZE  4096  // bytes - do not change this value

char diskname[48]; 
int  disk_fd; 
unsigned char volumesector[SECTORSIZE]; 
struct fat_boot_sector *bsp;

int get_sector (unsigned char *buf, int snum)
{
	off_t offset; 
	int n; 
	offset = snum * SECTORSIZE; 
	lseek (disk_fd, offset, SEEK_SET); 
	n  = read (disk_fd, buf, SECTORSIZE); 
	if (n == SECTORSIZE) 
		return (0); 
	else 
	{
		printf ("sector number %d invalid or read error.\n", snum); 
		exit (1); 
	}
}

void print_sector (unsigned char *s)
{
	int i;
	for (i = 0; i < SECTORSIZE; ++i) 
	{
		printf ("%02x ", (unsigned char) s[i]); 
		if ((i+1) % 16 == 0)
			printf ("\n"); 
	}
	printf ("\n");
}

int main(int argc, char *argv[])
{
	if (argc < 2) 
	{
		printf ("wrong usage\n"); 
		exit (1); 
	}

	strcpy (diskname, argv[1]); 
	disk_fd = open (diskname, O_RDWR); 
	if (disk_fd < 0) 
	{
		printf ("could not open the disk image\n"); 
		exit (1); 
	}

		get_sector (volumesector, 0); 
		unsigned int sec_size;
		unsigned int sec_fat;
		unsigned char sec_clus;
		unsigned char fats;
		unsigned short res_no;
		unsigned char* name = malloc(11);
		memcpy ( (void *) &sec_size, (void *) (volumesector + 11), 2);
		memcpy ( (void *) &sec_fat, (void *) (volumesector + 36), 4);
		memcpy ( (void *) &sec_clus, (void *) (volumesector + 13), 1);
		memcpy ( (void *) &fats, (void *) (volumesector + 16), 1);
		memcpy ( (void *) &res_no, (void *) (volumesector + 14), 2);
		memcpy ( (void *) name, (void *) (volumesector + 71), 11);
		unsigned int address = res_no + (fats * sec_fat);

		// unsigned int address = res_no + (fats * sec_fat);

	if(!(strcmp("volumeinfo", argv[3])))
	{
		printf("Volume ID information: \n");
		printf("Sector size: %u bytes\n",sec_size);
		printf("Sectors per FAT: %u\n",sec_fat);
		printf("Sectors per cluster: %u\n",sec_clus);
		printf("No. of FATs: %u\n",fats);
		printf("Volume ID: %s\n",name);
	}
	else if(!(strcmp("rootdir", argv[3])))
	{
		get_sector(volumesector,address);
		unsigned char check;
		memcpy ( (void *) &check, (void *) (volumesector), 1);
		int mul = 0;
		while(check != 0)
		{

				if(check != 229)
				{
					char* fdir_name = malloc(8);
					char* ext = malloc(3);
					unsigned int size;
					unsigned int low_cluster = 0;
					memcpy ( (void *) fdir_name, (void *) (volumesector + (mul * 32)), 8);
					memcpy ( (void *) ext, (void *) (volumesector + 8 + (mul * 32)), 3);
					memcpy ( (void *) &size, (void *) (volumesector + 28 + (mul * 32)), 4);
					memcpy ( (void *) &low_cluster, (void *) (volumesector + 26 + (mul * 32)), 2);
					printf("%s%s\n",fdir_name,ext);
					// printf("Name: %s, ext: %s, Size: %u bytes, Cluster Address(Low): %u\n", fdir_name,ext,size,low_cluster);
					mul++;
					memcpy ( (void *) &check, (void *) (volumesector + (mul * 32)), 1);
				}
				else
				{
					mul++;
					memcpy ( (void *) &check, (void *) (volumesector + (mul * 32)), 1);
				}
				
		
		}
	}
	else if(!(strcmp("blocks",argv[3])))
	{
		if(argv[4] == NULL)
		{
			printf("Please enter a file name\n");
			return 1;
		}
		int mul = 0;
		int c = 0;
		char* fname = malloc(sizeof(argv[4]));
		memcpy ( (void *) fname, (void *) (argv[4]),sizeof(argv[4]));
		int cluster = 0;
		int stopper = 65535;
		get_sector(volumesector,address);
		unsigned int fat_address = address + 1 * (sec_clus);
		unsigned char check;
		memcpy ( (void *) &check, (void *) (volumesector), 1);
		while(check != 0)
		{

			char* fdir_name = malloc(11);
			char* ext = malloc(3);
			memcpy ( (void *) fdir_name, (void *) (volumesector + (mul * 32)),8);
			memcpy ( (void *) ext, (void *) (volumesector + 8 + (mul * 32)), 3);
			fdir_name = strtok(fdir_name," ");
			strcat(fdir_name,".");
			strcat(fdir_name,ext);
			c = strcmp(fdir_name,argv[4]);
			if(!c)
			{
				break;
			}
			mul++;
			memcpy ( (void *) &check, (void *) (volumesector + (mul * 32)), 1);

		}
		if(c != 0)
		{
			printf("The file couldn't be found\n");
			return 1;
		}
		get_sector(volumesector,address);
		unsigned int val = 0;
		if(check != 229)
		{
			char* fdir_name = malloc(8);
			char* ext = malloc(3);
			unsigned int size;
			unsigned int low_cluster = 0;
			memcpy ( (void *) fdir_name, (void *) (volumesector + (mul * 32)), 8);
			memcpy ( (void *) ext, (void *) (volumesector + 8 + (mul * 32)), 3);
			memcpy ( (void *) &size, (void *) (volumesector + 28 + (mul * 32)), 4);
			memcpy ( (void *) &low_cluster, (void *) (volumesector + 26 + (mul * 32)), 2);
			printf("%s%s\n",fdir_name,ext);
			while(size > sec_size * sec_clus)
			{
                printf("%u : %x\n", low_cluster, low_cluster);
                low_cluster++;
                size = size - sec_size * sec_clus;
            }
            printf("%u : %x\n", low_cluster, low_cluster);
			mul++;
			memcpy ( (void *) &check, (void *) (volumesector + (mul * 32)), 1);
		}
		else
		{
			mul++;
			memcpy ( (void *) &check, (void *) (volumesector + (mul * 32)), 1);
		}
	}
	else
	{
		printf("Please enter a valid option\n");
		return 1;
	}
	close (disk_fd); 

	return (0); 
}