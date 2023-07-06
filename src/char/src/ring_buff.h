#ifndef  _LGZ_DEFINE_H_
#define  _LGZ_DEFINE_H_


struct lgz_ring_buff
{
    void * m_p_buff;
    char * m_p_data;
    int    m_size;
    int  * m_r;
    int  * m_w;
};

extern int lgz_init(struct lgz_ring_buff *p_this, int size);
extern void lgz_close(struct lgz_ring_buff *p_this);
extern int lgz_push_data(struct lgz_ring_buff *p_this, const char * p_data, int len);
extern int lgz_pop_data(struct lgz_ring_buff *p_this, char * p_data, int len);
extern int lgz_get_capacity(struct lgz_ring_buff *p_this);
extern int lgz_get_date_len(struct lgz_ring_buff *p_this);

#endif