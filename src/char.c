#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
#include <linux/skbuff.h>
#include <linux/ip.h>

static struct nf_hook_ops nfho;

/* This function will be called for each incoming packet */
static unsigned int capture_packet(void *priv, struct sk_buff *skb, const struct nf_hook_state *state)
{
    struct iphdr *ip_header;
    
    /* Check if the packet is an IP packet */
    if (skb->protocol == htons(ETH_P_IP)) {
        /* Get a pointer to the IP header */
        ip_header = (struct iphdr *)skb_network_header(skb);
        
        /* Print the source and destination IP addresses */
        printk(KERN_INFO "Captured packet: %pI4 -> %pI4  len:%d\n", &ip_header->saddr, &ip_header->daddr,cpu_to_be16(ip_header->tot_len));
    }
    
    /* Continue processing the packet */
    return NF_ACCEPT;
}

static int __init hello_init(void)
{
    nfho.hook = capture_packet;
    nfho.hooknum = NF_INET_PRE_ROUTING;
    nfho.pf = PF_INET;
    nfho.priority = NF_IP_PRI_FIRST;
    nf_register_hook(&nfho);
    printk(KERN_INFO "lyj hook Hello World!\n");
    return 0;
}

static void __exit hello_exit(void)
{
    nf_unregister_hook(&nfho);
    printk(KERN_INFO "lyj hook Goodbye World!\n");
}

module_init(hello_init);
module_exit(hello_exit);
MODULE_LICENSE("GPL");