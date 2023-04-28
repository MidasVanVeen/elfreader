#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>


static FILE *fp;

#define EI_NIDENT 16

typedef struct elfhdr32_t {
    uint16_t      e_type;
    uint16_t      e_machine;
    uint32_t      e_version;
    uint32_t      e_entry;
    uint32_t      e_phoff;
    uint32_t      e_shoff;
    uint32_t      e_flags;
    uint16_t      e_ehsize;
    uint16_t      e_phentsize;
    uint16_t      e_phnum;
    uint16_t      e_shentsize;
    uint16_t      e_shnum;
    uint16_t      e_shstrndx;
} elfhdr32;

typedef struct elfhdr64_t {
    uint16_t      e_type;
    uint16_t      e_machine;
    uint32_t      e_version;
    uint64_t      e_entry;
    uint64_t      e_phoff;
    uint64_t      e_shoff;
    uint32_t      e_flags;
    uint16_t      e_ehsize;
    uint16_t      e_phentsize;
    uint16_t      e_phnum;
    uint16_t      e_shentsize;
    uint16_t      e_shnum;
    uint16_t      e_shstrndx;
} elfhdr64;

void print_header(unsigned char ident[EI_NIDENT], uint8_t class, void *header) {
  printf("Ident:");
  for (int i=0; i<EI_NIDENT; i++) {
    printf(" %02X", ident[i]);
  }
  printf("\n");

  printf("Class: %02X, ", class);
  switch (class) {
    case 0x01:
      printf("32-bit");
      break;
    case 0x02:
      printf("64-bit");
      break;
  }
  printf("\n");

  printf("Data encoding: %02X, ", ident[5]);
  switch (ident[5]) {
    case 0x01:
      printf("Two's complement, little-endian");
      break;
    case 0x02:
      printf("Two's complement, big-endian");
      break;
    case 0x00:
      printf("Unknown data format");
      break;
  }
  printf("\n");

  printf("EI version: %02X, ", ident[6]);
  if (0x01 == ident[6]) {
    printf("Current");
  } else {
    printf("Invalid");
  }
  printf("\n");

  printf("OSABI: %02X, ", ident[7]);
  switch (ident[7]) {
    case 0x00:
      printf("No extension or unspecified");
      break;
    case 0x01:
      printf("HP-UX");
      break;
    case 0x02:
      printf("NetBSD");
      break;
    case 0x03:
      printf("Linux");
      break;
    case 0x06:
      printf("Sun Solaris");
      break;
    case 0x07:
      printf("AIX");
      break;
    case 0x08:
      printf("IRIX");
      break;
    case 0x09:
      printf("FreeBSD");
      break;
    case 0x0a:
      printf("TRU64 UNIX");
      break;
    case 0x0b:
      printf("Novell Modesto");
      break;
    case 0x0c:
      printf("Open BSD");
      break;
    case 0x0d:
      printf("Open VMS");
      break;
    case 0x61:
      printf("ARM");
      break;
    case 0xff:
      printf("Standalone (embedded) application");
      break;
  }
  printf("\n\n");

  elfhdr32 *header32 = header;
  printf("Type: %08X, ", header32->e_type);
  switch ((uint8_t)header32->e_type) {
    case 0:
      printf("Unknown file");
      break;
    case 1:
      printf("Relocatable file");
      break;
    case 2:
      printf("Executable file");
      break;
    case 3:
      printf("Shared object");
      break;
    case 4:
      printf("Core file");
      break;
  }
  printf("\n");

  printf("Machine: %08X, ", header32->e_machine);
  switch (header32->e_machine) {
    case 0:
      printf("Unknowe machine");
      break;
    case 1:
      printf("AT&T WE 32100");
      break;
    case 2:
      printf("Sun Microsystems SPARC");
      break;
    case 3:
      printf("Intel 80386");
      break;
    case 4:
      printf("Motorola 68000");
      break;
    case 5:
      printf("Motorola 88000");
      break;
    case 6:
      printf("Intel 80860");
      break;
    case 7:
      printf("Intel i860");
      break;
    case 8:
      printf("MIPS RS3000 Big-Endian");
      break;
    case 9:
      printf("IBM System/370 Processor");
      break;
    case 10:
      printf("MIPS RS3000 Little-Endian");
      break;
    case 11:
      printf("RS6000");
      break;
    default:
      printf("Unknown machine*");
  }
  printf("\n");

  if (0x02 == class) {
    elfhdr32 *header64 = header;
  }
}

int main(int argc, char *argv[]) {
  if (argc < 2) {
    fprintf(stderr, "Not enough arguments supplied!\n");
    exit(1);
  }
  fp = fopen(argv[1], "r");
  if (NULL == fp) {
    perror("Fatal error");
    exit(1);
  }

  unsigned char e_ident[EI_NIDENT];
  int bytesread = fread(e_ident, sizeof(char), EI_NIDENT, fp);
  if (EI_NIDENT != bytesread) {
    fprintf(stderr, "Fatal error while reading magic bytes\n");
    exit(1);
  }

  char magicbytes[5];
  const char correctmagicbytes[5] = { 0x7f, 'E', 'L', 'F', 0};
  memcpy(magicbytes, e_ident, 4);
  if (strcmp(correctmagicbytes, magicbytes)) {
    fprintf(stderr, "Supplied file is NOT an elf file. (wrong magicbytes)\n");
    exit(1);
  }

  uint8_t e_class = e_ident[4];
  void *finalheader;

  if (0x01 == e_class) {
    // class == 32-bit
    elfhdr32 *ElfHeader = malloc(sizeof(elfhdr32));
    bytesread = fread(ElfHeader, sizeof(char), sizeof(elfhdr32)/sizeof(char), fp);
    if (sizeof(elfhdr32) != bytesread) {
        fprintf(stderr, "Fatal error while reading elfheader\n");
        exit(1);
    }
    finalheader = ElfHeader;
  } else if (0x02 == e_class) {
    // class == 64-bit
    elfhdr64 *ElfHeader = malloc(sizeof(elfhdr64));
    bytesread = fread(ElfHeader, sizeof(char), sizeof(elfhdr64), fp);
    if (sizeof(elfhdr64) != bytesread) {
        fprintf(stderr, "Fatal error while reading elfheader\n");
        exit(1);
    }
    finalheader = ElfHeader;
  } else {
    fprintf(stderr, "Invalid efl file: class is invalid\n");
    exit(1);
  }

  print_header(e_ident, e_class, finalheader);

  free(finalheader);
  return 0;
}
