SECTIONS {
    . = 0x80100000;

    . = ALIGN(4);
    .text      :
    {
      *(.text)
    }

    . = ALIGN(4);
    .rodata : { *(.rodata) }

    . = ALIGN(4);

    data_load_addr = .;
    .data 0x900000 : AT(data_load_addr)
    {
      data_start = . ;
      *(.data) 
      data_end = . ;
    }

    . = ALIGN(4);
    __bss_start = .;
    .bss : { *(.bss) *(.COMMON) }
    __bss_end = .;
}
		  			 		  						  					  				 	   		  	  	 	  