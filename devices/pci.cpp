//////////////////////////////////////////////////////////
// This file is a part of Nanos Copyright (C) 2008-2010 //
// ashok.s.das@gmail.com                                //
//////////////////////////////////////////////////////////
// PCI implementation                                   //
//                                                      //
//////////////////////////////////////////////////////////

#include "pci.h"
#include "pcidefs.h"
#include "low-io.h"
#include "OStream.h"
#include "string.h"
//using namespace std;
using namespace String;
// reads pci bus
// takes bus number device number function number register and type
// returns the read value from the above pci description 
// Note: the return value is dependant on type(uint8_ts) if it is 1
//       then a byte is returned, if it is 2 a word value is return
//       if it is 4 a dword(long int) is returned. 
unsigned int read_pci(int bus, int dev, int func, int reg, int uint8_ts)
{
	unsigned short base;

	union {
		confadd_t c;
		unsigned int n;
	} u;

	u.n = 0;
	u.c.enable = 1;
	u.c.rsvd = 0;
	u.c.bus = bus;
	u.c.dev = dev;
	u.c.func = func;
	u.c.reg = reg & 0xFC;

	outportl(PCI_DATA_REG, u.n);
	base = PCI_BASE_REG + (reg & 0x03);

	switch(uint8_ts)
	{
		case 1: return inportb(base);
		case 2: return inportw(base);
		case 4: return inportl(base);
		default: return 0;
	}
}
// writes a config byte/word/dword to 
// bus device function register with v= value to write and 
// size of the write operation
// takes bus device function register value to write and type
// Note: Type is described above
void write_pci(int bus, int dev, int func, int reg, unsigned int v, int uint8_ts)
{
	unsigned short base;

	union {
		confadd_t c;
		unsigned int n;
	} u;

	u.n = 0;
	u.c.enable = 1;
	u.c.rsvd = 0;
	u.c.bus = bus;
	u.c.dev = dev;
	u.c.func = func;
	u.c.reg = reg & 0xFC;

	base = PCI_BASE_REG + (reg & 0x03);
	outportl(PCI_DATA_REG, u.n);

	switch(uint8_ts)
	{
		case 1: outportb(base, (unsigned char) v); break;
		case 2: outportw(base, (unsigned short) v); break;
		case 4: outportl(base, v); break;
	}
}
//wraper functions for read and write
unsigned char pci_read_config_byte(int bus, int dev, int func, int reg)
{
	return ( read_pci(bus, dev, func, reg, sizeof(unsigned char)) );
}
unsigned short pci_read_config_word(int bus, int dev, int func, int reg)
{
	return ( read_pci(bus, dev, func, reg, sizeof(unsigned short)) );
}
unsigned int pci_read_config_dword(int bus, int dev, int func, int reg)
{
	return ( read_pci(bus, dev, func, reg, sizeof(unsigned int)) );
}
void pci_write_config_byte(int bus, int dev, int func, int reg, unsigned char val)
{
	write_pci(bus, dev, func, reg, val, sizeof(unsigned char));
}
void pci_write_config_word(int bus, int dev, int func, int reg, unsigned short val)
{
	write_pci(bus, dev, func, reg, val, sizeof(unsigned short));
}
void pci_write_config_dword(int bus, int dev, int func, int reg, unsigned int val)
{
	write_pci(bus, dev, func, reg, val, sizeof(unsigned int));
}
unsigned char pci_read_irq(int bus,int dev,int func)
{
	unsigned char irq = 0;

	irq = pci_read_config_byte(bus, dev, func, PCI_INTERRUPT_PIN);
	cout<<"DEBUG"<<(unsigned int)irq<<" pin\n";
	if (irq)
		irq = pci_read_config_byte(bus, dev, func, PCI_INTERRUPT_LINE);
	cout<<"DEBUG"<<(unsigned int)irq<<" line\n";
	return irq;
}
// checks if a PCI bus is present, for further clarification see the 
// link bellow
// takes nothing 
// returns 0 if false 1 if true
bool is_pci_present()
{
//	taken from http://www.waste.org/~winkles/hardware/pci.htm
	unsigned int res=inportl(PCI_DATA_REG);
#ifdef __DEBUG_
	cout.flags(hex|showbase);
	cout<<res<<"\n";
	cout.flags(dec);
#endif
	if(res==0xFFFFFFFF)
		return false;
	else
		return true;
}
// converts classcode and subclass to coresponding sring
// takes classcode and subclass
// returns string
char *class_to_string(unsigned char classcode,unsigned char subclass)
{
	char *cls_str;
	unsigned int code;
	cls_str=new char[40];
	
	code = (classcode<<8)+subclass;
	switch(code)
	{
		case PCI_CLASS_NOT_DEFINED : strcpy(cls_str,"Unknown device/VGA ");
				break;
		case PCI_CLASS_NOT_DEFINED_VGA : strcpy(cls_str,"Unknown/VGA ");
				break;
		case PCI_CLASS_STORAGE_SCSI : strcpy(cls_str,"SCSI-Disk ");
				break;
		case PCI_CLASS_STORAGE_IDE : strcpy(cls_str,"IDE-Disk ");
				break;
		case PCI_CLASS_STORAGE_FLOPPY : strcpy(cls_str,"Floppy-Disk ");
				break;
		case PCI_CLASS_STORAGE_IPI : strcpy(cls_str,"IPI-Disk ");
				break;
		case PCI_CLASS_STORAGE_RAID : strcpy(cls_str,"RAID ");
				break;
		case PCI_CLASS_STORAGE_SATA : strcpy(cls_str,"SATA ");
				break;
		case PCI_CLASS_STORAGE_SATA_AHCI : strcpy(cls_str,"SATA AHCI ");
				break;
		case PCI_CLASS_STORAGE_SAS : strcpy(cls_str,"SAS ");
				break;
		case PCI_CLASS_STORAGE_OTHER : strcpy(cls_str,"Other Storage ");
				break;
		case PCI_CLASS_NETWORK_ETHERNET : strcpy(cls_str,"Ethernet ");	
				break;
		case PCI_CLASS_NETWORK_TOKEN_RING : strcpy(cls_str,"Tokenring ");	
				break;
		case PCI_CLASS_NETWORK_FDDI : strcpy(cls_str,"FDDI ");	
				break;
		case PCI_CLASS_NETWORK_ATM : strcpy(cls_str,"ATM ");	
				break;
		case PCI_CLASS_NETWORK_OTHER : strcpy(cls_str,"Other ");	
				break;
		case PCI_CLASS_DISPLAY_VGA : strcpy(cls_str,"VGA ");
				break;
		case PCI_CLASS_DISPLAY_XGA : strcpy(cls_str,"XGA ");	
				break;
		case PCI_CLASS_DISPLAY_3D : strcpy(cls_str,"3D ");	
				break;
		case PCI_CLASS_DISPLAY_OTHER : strcpy(cls_str,"Other Disp ");	
				break;
		case PCI_CLASS_MULTIMEDIA_VIDEO : strcpy(cls_str,"Video ");
				break;
		case PCI_CLASS_MULTIMEDIA_AUDIO : strcpy(cls_str,"Audio ");	
				break;
		case PCI_CLASS_MULTIMEDIA_PHONE : strcpy(cls_str,"Phone ");	
				break;
		case PCI_CLASS_MULTIMEDIA_OTHER : strcpy(cls_str,"Other Mmedia ");	
				break;
		
		case PCI_CLASS_MEMORY_RAM : strcpy(cls_str,"RAM ");
				break;
		case PCI_CLASS_MEMORY_FLASH : strcpy(cls_str,"FLASH ");
				break;
		case PCI_CLASS_MEMORY_OTHER : strcpy(cls_str,"Other Mem ");
				break;
		case PCI_CLASS_BRIDGE_HOST : strcpy(cls_str,"Host Bridge ");
				break;
		case PCI_CLASS_BRIDGE_ISA : strcpy(cls_str,"ISA Bridge ");
				break;
		case PCI_CLASS_BRIDGE_EISA : strcpy(cls_str,"EISA Bridge ");
				break;
		case PCI_CLASS_BRIDGE_MC : strcpy(cls_str,"MC Bridge ");
				break;
		case PCI_CLASS_BRIDGE_PCI : strcpy(cls_str,"PCI Bridge ");
				break;
		case PCI_CLASS_BRIDGE_PCMCIA : strcpy(cls_str,"PCMCIA Bridge ");
				break;
		case PCI_CLASS_BRIDGE_NUBUS : strcpy(cls_str,"NUBUS Bridge ");
				break;
		case PCI_CLASS_BRIDGE_CARDBUS : strcpy(cls_str,"CardBus Bridge ");
				break;
		case PCI_CLASS_BRIDGE_RACEWAY : strcpy(cls_str,"Raceway Bridge ");
				break;
		case PCI_CLASS_BRIDGE_OTHER : strcpy(cls_str,"Other Bridge ");
				break;
		case PCI_CLASS_COMMUNICATION_SERIAL : strcpy(cls_str,"Serial ");
				break;
		case PCI_CLASS_COMMUNICATION_PARALLEL : strcpy(cls_str,"Parallel ");
				break;
		case PCI_CLASS_COMMUNICATION_MULTISERIAL : strcpy(cls_str,"MultiSerial ");
				break;
		case PCI_CLASS_COMMUNICATION_MODEM : strcpy(cls_str,"Modem ");
				break;
		case PCI_CLASS_COMMUNICATION_OTHER : strcpy(cls_str,"Other Comm ");
				break;
		case PCI_CLASS_SYSTEM_PIC : strcpy(cls_str,"PIC ");
				break;
		case PCI_CLASS_SYSTEM_PIC_IOAPIC : strcpy(cls_str,"IOAPIC ");
				break;
		case PCI_CLASS_SYSTEM_PIC_IOXAPIC : strcpy(cls_str,"IOXAPIC ");
				break;
		case PCI_CLASS_SYSTEM_DMA : strcpy(cls_str,"DMA ");
				break;
		case PCI_CLASS_SYSTEM_TIMER : strcpy(cls_str,"Timer ");
				break;
		case PCI_CLASS_SYSTEM_RTC : strcpy(cls_str,"RTC ");
				break;
		case PCI_CLASS_SYSTEM_PCI_HOTPLUG : strcpy(cls_str,"PCI Hotplug ");
				break;
		case PCI_CLASS_SYSTEM_SDHCI : strcpy(cls_str,"SDHCI ");
				break;
		case PCI_CLASS_SYSTEM_OTHER : strcpy(cls_str,"Other System ");
				break;
		case PCI_CLASS_INPUT_KEYBOARD : strcpy(cls_str,"Keyboard ");
				break;
		case PCI_CLASS_INPUT_PEN : strcpy(cls_str,"Pen ");
				break;
		case PCI_CLASS_INPUT_MOUSE : strcpy(cls_str,"Mouse ");
				break;
		case PCI_CLASS_INPUT_SCANNER : strcpy(cls_str,"SCANNER ");
				break;
		case PCI_CLASS_INPUT_GAMEPORT : strcpy(cls_str,"Gameport ");
				break;
		case PCI_CLASS_INPUT_OTHER : strcpy(cls_str,"Other Input ");
				break;
		case PCI_CLASS_DOCKING_GENERIC : strcpy(cls_str,"Docking generic ");
				break;
		case PCI_CLASS_DOCKING_OTHER : strcpy(cls_str,"Docking Other ");
				break;
		case PCI_CLASS_PROCESSOR_386 : strcpy(cls_str,"i386 ");
				break;
		case PCI_CLASS_PROCESSOR_486 : strcpy(cls_str,"i486 ");
				break;
		case PCI_CLASS_PROCESSOR_PENTIUM : strcpy(cls_str,"Pentium ");
				break;
		case PCI_CLASS_PROCESSOR_ALPHA : strcpy(cls_str,"Alpha ");
				break;
		case PCI_CLASS_PROCESSOR_MIPS : strcpy(cls_str,"MIPS ");
				break;
		case PCI_CLASS_PROCESSOR_CO : strcpy(cls_str,"CO??? ");
				break;
		
		case PCI_CLASS_SERIAL_FIREWIRE : strcpy(cls_str,"Firewire ");
				break;
		case PCI_CLASS_SERIAL_FIREWIRE_OHCI : strcpy(cls_str,"Firewire-OHCI ");
				break;
		case PCI_CLASS_SERIAL_SSA : strcpy(cls_str,"SSA ");
				break;
		
		case PCI_CLASS_SERIAL_USB : strcpy(cls_str,"USB ");
				break;
		case PCI_CLASS_SERIAL_USB_UHCI : strcpy(cls_str,"USB UHCI ");
				break;
		case PCI_CLASS_SERIAL_USB_OHCI : strcpy(cls_str,"USB OHCI ");
				break;
		case PCI_CLASS_SERIAL_USB_EHCI : strcpy(cls_str,"USB EHCI ");
				break;
		case PCI_CLASS_SERIAL_FIBER : strcpy(cls_str,"Fiber ");
				break;
		case PCI_CLASS_SERIAL_SMBUS : strcpy(cls_str,"SMBUS ");
				break;
		
		case PCI_CLASS_WIRELESS_RF_CONTROLLER : strcpy(cls_str,"Wireless rf control ");
				break;
		case PCI_CLASS_WIRELESS_WHCI : strcpy(cls_str,"Wireless rf control ");
				break;
		
		case PCI_CLASS_INTELLIGENT_I2O : strcpy(cls_str,"Intelligent I2O ");
				break;
		case PCI_CLASS_SATELLITE_TV : strcpy(cls_str,"Satellite Tv ");
				break;
		case PCI_CLASS_SATELLITE_AUDIO : strcpy(cls_str,"Satellite Audio ");
				break;
		case PCI_CLASS_SATELLITE_VOICE : strcpy(cls_str,"Satellite Voice ");
				break;
		case PCI_CLASS_SATELLITE_DATA : strcpy(cls_str,"Satellite Data ");
				break;
		case PCI_CLASS_CRYPT_NETWORK : strcpy(cls_str,"Crypt Network");
				break;
		case PCI_CLASS_CRYPT_ENTERTAINMENT : strcpy(cls_str,"Crypt Entertainment ");
				break;
		case PCI_CLASS_CRYPT_OTHER : strcpy(cls_str,"Crypt Other ");
				break;
		case PCI_CLASS_SP_DPIO : strcpy(cls_str,"DPIO ");
				break;
		case PCI_CLASS_SP_OTHER : strcpy(cls_str,"DSP Other  ");
				break;
		default: strcpy(cls_str,"Invalid class subclass");
	}
	return cls_str;
}
// returns vendor name
// takes vendor ID
// returns string
const char *vendor_to_string(unsigned short vend_id)
{
	const char vend_str[]="Unknown Vendor";
	for(unsigned int i=0;i<PCI_VENTABLE_LEN;i++)
	{
		if(vendor_array[i].ven_id==vend_id)
			return vendor_array[i].vendor;
	}
	return vend_str;
}
const char *vendor_device_to_string(unsigned short vendor,unsigned short device)
{
	const char dev_str[]="Unknown Device";
	for(unsigned int i=0;i<PCI_DEVICE_TABLE_LEN;i++)
	{
		if(pcibus_device_names[i].vendor==vendor && pcibus_device_names[i].device==device)
			return pcibus_device_names[i].device_name;
	}
	return dev_str;
}
#if 0
// scans the pci-bus
// takes nothing
// returns nothing
void pci_bus_scan()
{
	int bus,dev,fun,tot_pci_dev=0;
	unsigned int temp[4];
	struct PCI_common *cfg;
	if(!is_pci_present()) 
	{
		cout<<"No PCI bus ...\n";
		return;
	}
	for(bus=0;bus<255;bus++)
	{
		for(dev=0;dev<32;dev++)
		{
			for(fun=0;fun<8;fun++)
			{
				for(int i=0;i<4;i++)
					temp[i]=pci_read_config_dword(bus,dev,fun,i<<2);
				cfg=(struct PCI_common *)temp;
				if(cfg->vendor_id==0xFFFF)
				continue;
				tot_pci_dev++;
				cout.flags(hex|showbase);
				cout<<bus<<":"<<dev<<":"<<fun \
				    <<" vend="<<vendor_to_string(cfg->vendor_id)<<" devid="<<cfg->device_id \
				    <<" cls="<<class_to_string(cfg->classcode,cfg->subclass) \
				    <<(int)cfg->classcode<<" sbcls="<<(int)cfg->subclass<<" master" \
				    <<(int)(cfg->Prog_if&(1<<7))<<"\n";
				cout.flags(dec);
			}
		}
	}
	cout<<"Total PCI devices discovered ="<<tot_pci_dev<<"\n";
	cout<<"size of common="<<sizeof(struct PCI_common)<<" non bridge="<<sizeof(struct non_bridge) \
            <<" bridge="<<sizeof(struct bridge)<<" card bus="<<sizeof(struct cardbus)<<"\n";
}
#endif
void pci_bus::scan()
{
	unsigned int bus,dev,fun,num_fun;
	//register unsigned int *temp = new unsigned int[4];
	PCI_common *cfg;
	pci_dev *pd;
	if(!is_pci_present())
	{
		cout<<"No PCI bus...!\n";
		return;
	}
	for(bus=0;bus<255;bus++)
	{
		for(dev=0;dev<32;dev++)
		{
			if(pci_read_config_byte(bus,dev,0,0x0e)==0x80)
				num_fun=8;
			else 
				num_fun=1;

			for(fun=0;fun<num_fun;fun++)
			{
				unsigned int *temp=new unsigned int[4];
				memset(temp,'0',4*sizeof(unsigned int));
				for(int i=0;i<4;i++)
				{
					temp[i]=pci_read_config_dword(bus,dev,fun,i<<2);
				}
				cfg=(struct PCI_common*)temp;
				if((cfg->vendor_id==0xffff) || (cfg->vendor_id==0x0000))
				{
					delete[] temp;
					continue;
				}
				num_dev++;
				pd=new pci_dev;
				if(!pd) halt();
				pd->bus=bus;
				pd->dev=dev;
				pd->func=fun;
				for(int i=0;i<60;i++)
					pd->devi[i]=pci_read_config_dword(bus,dev,fun,(i<<2)+16);
				pd->prev=NULL;
				pd->next=NULL;
				pd->common=cfg;
				pd->irq = pci_read_irq(bus,dev,fun);
				pci_set_master(pd);
				if(pci_list==NULL)
				{
					pci_list=pd;
					end=pci_list;
					pci_list->next=NULL;
					pci_list->prev=NULL;
				}
				else
				{
					pd->prev=end;
					end->next=pd;
					pd->next=NULL;
					end=pd;
				}
				cout<<"*";
			}// end fun
		}// end dev
	}//end bus
	cout<<"\n";
}
void pci_bus::list_dev()
{
	pci_dev *device=pci_list;
	while(device)
	{
		cout.flags(hex|showbase);
		cout<<device->bus<<":"<<device->dev<<":"<<device->func<<":";
		cout<<vendor_to_string(device->common->vendor_id)<<":"<<vendor_device_to_string(device->common->vendor_id,device->common->device_id)<<":";
		cout<<class_to_string(device->common->classcode,device->common->subclass)<<":";
		cout<<(int)device->common->classcode<<":"<<(int)device->common->subclass<<":";
		cout<<(int)(device->common->Prog_if)<<":"<<(int)device->common->header_type<<" irq:";
		cout<<(int)device->irq<<"\n";
		device=device->next;
	}
	cout.flags(dec);
	cout<<"Total pci devices="<<num_dev<<"\n";
}
// the 2 functions will return a pci_dev if present else NULL
// one takes vendor and dev_id if a match is found then returns pci_dev
// the other takes a class and subclass and returns pci_dev if found
pci_dev *pci_bus::get_dev(unsigned short vendor,unsigned short device)
{
	pci_dev *temp;
	temp=pci_list;
	while(temp)
	{
		if(temp->common->vendor_id==vendor && temp->common->device_id==device)
			return temp;
		temp=temp->next;
	}
	return NULL;
}
pci_dev *pci_bus::get_dev_by_class(unsigned char classcode)
{
	pci_dev *temp,*temp2=NULL,*curr;
	temp=pci_list;
	while(temp)
	{
		if(temp->common->classcode==classcode)
		{
			if(temp2==NULL)
			{
				temp2=new pci_dev;
				memcpy(temp2,temp,sizeof(pci_dev));
				curr=temp2;
				curr->next=NULL;
			}
			else
			{
				curr->next=new pci_dev;
				memcpy(curr->next,temp,sizeof(pci_dev));
				curr=curr->next;
				curr->next=NULL;
			}
		}
		temp=temp->next;
	}
	curr->next=NULL;
	return temp2;
}	
pci_dev *pci_bus::get_dev(unsigned char classcode,unsigned char subclass)
{
	pci_dev *temp,*temp2=NULL,*curr;
	temp=pci_list;
	while(temp)
	{
		if(temp->common->classcode==classcode && temp->common->subclass==subclass)
		{
			if(temp2==NULL)
			{
				temp2=new pci_dev;				
				memcpy(temp2,temp,sizeof(pci_dev));
				curr=temp2;
				curr->next=NULL;
			}
			else
			{
				curr->next=new pci_dev;
				memcpy(curr->next,temp,sizeof(pci_dev));
				curr=curr->next;
				curr->next=NULL;
			}
		//	return temp;
		}
		temp=temp->next;
	}
	curr->next=NULL;
	return temp2;
}
bar_type_t get_bar_type(pci_dev *dev,int bar_num)
{
	unsigned int temp;
	temp=pci_read_config_dword(dev->bus,dev->dev,dev->func,0x10+(bar_num<<2));
	if(temp & 0x00000001)
	{
		cout<<"IO";
		return BAR_TYPE_IO;
	}
	cout<<"MEM";
	return BAR_TYPE_MEM;
}
unsigned int get_bar(pci_dev *dev,int bar_num)
{
	unsigned int temp;
	temp=pci_read_config_dword(dev->bus,dev->dev,dev->func,0x10+(bar_num<<2));
	if(temp & 0x00000001)
		return temp & (~0x03);
	return temp & (~0x0F);
}
void pci_set_master(pci_dev *pdev)
{
	unsigned short cmd;
	unsigned char lat;

	cmd = pci_read_config_word(pdev->bus, pdev->dev, pdev->func, PCI_COMMAND);
	if ( !(cmd & PCI_COMMAND_MASTER) )
	{
		cout<<"PCI: Enabling bus mastering for device in slot "<<(unsigned short) pdev->bus<<" " \
			<<(unsigned short)pdev->dev<<" "<<(unsigned short) pdev->func<<"\n";
		cmd |= PCI_COMMAND_MASTER;
		pci_write_config_word(pdev->bus, pdev->dev, pdev->func, PCI_COMMAND, cmd);
	}
	// Check the latency time, because certain BIOSes forget to set	//
	// it properly...						//
	lat = pci_read_config_byte(pdev->bus, pdev->dev, pdev->func, PCI_LATENCY_TIMER);
	if ( lat < 16 )
		lat =  255; // 255 is the max latency
	else if ( lat > 255 )
		lat = 255;
	else
		return;
	cout<<"PCI: Setting latency timer of device "<<(unsigned short) pdev->bus<<" "<<(unsigned short)pdev->dev \
		<<" "<<(unsigned short) pdev->func<<" to "<<(unsigned short) lat<<"\n";
	pci_write_config_byte(pdev->bus, pdev->dev, pdev->func, PCI_LATENCY_TIMER, lat);
	cout<<"Enabling Device IO\n";
	if(pci_enable_device_io(pdev) != 0)
		cout<<"WARNING : Unable to enable device... should be enabled latter\n"; 
}

#if 0// set the power state of a device
//int pci_set_power_state(pci_cfg_t *cfg, int state)
int pci_set_pwr_state(pci_dev *cfg, int state)
{
	int pm;
	unsigned short pmcsr;
	unsigned short pmc;

	// Bound the state to a valid range				//
	if (state > 3) state = 3;

	// Validate current state.					//
	// Can enter D0 from any state, but we can't go deeper if we're //
	// in a low power state.					//
	if (state > 0 && cfg->current_state > state)
		return(-1); //invalid einval
	else if (cfg->current_state == state)
		// we're already there 					//
		return(0);

	// find PCI PM capability in list 				//
	pm = pci_find_capability(cfg, PCI_CAP_ID_PM);

	// Abort if the device doesn't support PM capabilities		//
	if (!pm) return(-2); //error in IO 

	// Check if this device supports the desired state		//
	if (state == 1 || state == 2)
	{
		pmc = pci_read_config_word(cfg->bus, cfg->dev, cfg->func, pm+PCI_PM_PMC);
		if ( (state == 1 && !(pmc & PCI_PM_CAP_D1)) )
			return(-2); //error IO
		else if ( (state == 2 && !(pmc & PCI_PM_CAP_D2)) )
			return(-2); //error IO
	}

	// If we're in D3, force entire word to 0.			//
	// This doesn't affect PME_Status, disables PME_En, and		//
	// sets PowerState to 0.					//
	if ( cfg->current_state>=3 )
		pmcsr = 0;
	else
	{
		pmcsr = pci_read_config_word(cfg->bus, cfg->dev, cfg->func, pm+PCI_PM_CTRL);
		pmcsr &= ~PCI_PM_CTRL_STATE_MASK;
		pmcsr |= state;
	}

	// Enter specified state //
	pci_write_config_word(cfg->bus, cfg->dev, cfg->func, pm+PCI_PM_CTRL, pmcsr);

	// Mandatory power management transition delays			//
	// see PCI PM 1.1 5.6.1 table 18				//
	if( (state == 3) || (cfg->current_state == 3) )
	{
		// Set task state to interruptible			//
		// LINUX do it so:					//
		// 	set_current_state(TASK_UNINTERRUPTIBLE);	//
		// 	schedule_timeout(HZ/100);			//
		delay(100);
	}
	else if( (state == 2) || (cfg->current_state == 2) )
		// udelay(200);
		delay(200);
	cfg->current_state = state;

	return(0);
}
#endif
unsigned char pci_enable_device_io(pci_dev *cfg)
{
	unsigned short cmd, old_cmd;
	cout<<"\n\rLow level enabling PCI device<<"<<(unsigned short)cfg->bus<<(unsigned short)cfg->dev<<(unsigned short)cfg->func;

	old_cmd = cmd = pci_read_config_word(cfg->bus, cfg->dev, cfg->func, PCI_COMMAND);
	for (int i=0; i<6; i++)
	{
		if (get_bar(cfg, i) == PCI_COMMAND_IO)
			// Command IO based				//
			cmd |= PCI_COMMAND_IO;
	}
	if ( !(cmd & PCI_COMMAND_IO) )
	{
		// Device is not IO-based				//
		cout<<"\n\rDevice is not IO-based!!!";
		return(0xe); //invalid einval
	}

	if ( (cfg->common->header_type & 0x7F) == PCI_HEADER_TYPE_BRIDGE )
	{
		// Any PCI-to-PCI bridge must be enabled by setting	//
		// both I/O space and memory space access bits in the	//
		// command register.					//
		cmd |= PCI_COMMAND_MEMORY;
	}

	// Always enable bus master!!!					//
	cmd |= PCI_COMMAND_MASTER;

	if ( cmd!=old_cmd )
	{
		// Set the cache line and default latency (32)			//
		pci_write_config_word(cfg->bus, cfg->dev, cfg->func,
				PCI_CACHE_LINE_SIZE, (32 << 8) | (L1_CACHE_BYTES / sizeof(unsigned int)));
		// Enable the appropriate bits in the PCI command register	//
		pci_write_config_word(cfg->bus, cfg->dev, cfg->func, PCI_COMMAND, cmd);
		cout<<"OK!\n";
	}
	else
		cout<<"Already enabled.\n";
	return(0);
}
#if 0
//enables a device for IO.
int pci_enable_device(pci_dev *cfg)
{
	int err, pm;

	cout<<"Powering on PCI device"<<(unsigned short)cfg->bus<<(unsigned short) cfg->dev<<(unsigned short) cfg->func<<"\n";
	pm = pci_set_power_state(cfg, 0);
	switch( pm )
	{
		case 0:
		cout<<"OK!";
		break;

		case (-2):
		cout<<"Device doesn't support Power Management Capabilities!!!";
		break;

		case (-1):
		cout<<"\n\rDevice is already in this power state.";
		break;
	}

	if ((err = pcibios_enable_device_io(cfg)) < 0)
		return(err);
	return(0);
}
#endif
