#include <stdio.h>
#include <pcap.h>

int main(int argc, char *argv[]) {
	char *dev;
	char errbuf[PCAP_ERRBUF_SIZE];
	pcap_t *handle;

	bpf_u_int32 mask;
	bpf_u_int32 net;

	struct bpf_program fp;

	char filter_exp[] = "port 80";
	struct pcap_pkthdr header;
	const u_char *packet; 

	/*Define the device*/
	dev = pcap_lookupdev(errbuf);
	if (dev == NULL) {
		fprintf(stderr, "Could not find default device: %s\n", errbuf);
		return(2);
	}
	printf("Device: %s\n", dev);

	/*Fine the properties for eht device*/
	if (pcap_lookupnet(dev, &net, &mask, errbuf) == -1) {
		fprintf(stderr, "Could not get netmask for device %s: %s\n", dev, errbuf);
		net = 0; 
		mask = 0;
	}

	/*Open the session in promiscuous mode*/
	handle = pcap_open_live(dev, BUFSIZ, 1, 1000, errbuf);
	if (handle == NULL) {
		fprintf(stderr, "Could not open device %s: %s\n", dev, errbuf);
		return(2);
	}

	/*Compile and apply the filter*/
	if (pcap_compile(handle, &fp, filter_exp, 0, net) == -1) {
		fprintf(stderr, "Could not parse filter %s: %s\n", filter_exp, pcap_geterr(handle));
		return(2);
	}
	if (pcap_setfilter(handle, &fp) == -1) {
		fprintf(stderr, "Could not install filter %s: %s\n", filter_exp, pcap_geterr(handle));
		return(2);
	}

	/*Grab a packet*/
	packet = pcap_next(handle, &header);
	/*Print this packet's length*/
	printf("length of: %d\n", header.len);

	/*Close the session*/
	pcap_close(handle);
	return(0);
}