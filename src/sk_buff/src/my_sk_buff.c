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
    struct my_netdev_priv *netdev_priv = priv;
    struct ethhdr *eth_header;

    struct iphdr *ip_header;
    
    if (!skb) {
        return NF_ACCEPT; 
    }

    
    // if (state->hook != NF_INET_PRE_ROUTING) {
    //     return NF_ACCEPT;
    // }

    eth_header = eth_hdr(skb);

    // 打印以太网帧的内容
    printk(KERN_INFO "Eth Frame: SMAC: %pM DMAC: %pM Protocol: %04X \n", eth_header->h_source,eth_header->h_dest, ntohs(eth_header->h_proto));

    return NF_ACCEPT;
}

static int __init hello_init(void)
{
    nfho.hook = capture_packet;
    nfho.hooknum = NF_INET_PRE_ROUTING;
    nfho.pf = PF_INET;
    nfho.priority = NF_IP_PRI_FIRST;
    nf_register_net_hook(&init_net,&nfho);
    printk(KERN_INFO "lyj hook Hello World!\n");
    return 0;
}

static void __exit hello_exit(void)
{
    nf_unregister_net_hook(&init_net,&nfho);
    printk(KERN_INFO "lyj hook Goodbye World!\n");
}

module_init(hello_init);
module_exit(hello_exit);
MODULE_LICENSE("GPL");