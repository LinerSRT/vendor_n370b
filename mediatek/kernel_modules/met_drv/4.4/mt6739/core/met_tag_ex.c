#define BUILD_WITH_MET

#ifdef MET_USER_EVENT_SUPPORT

#include <linux/kernel.h>
#include <linux/mutex.h>
#include <linux/slab.h>
#include <linux/mm.h>
#include <linux/fs.h>
/* #include <asm/uaccess.h> */
#include <linux/uaccess.h>

#include "met_drv.h"
#include "met_tag.h"
#include "interface.h"
#include "switch.h"
#include "met_api_tbl.h"

struct bltable_t bltab;

static int dump_buffer_size;
static int dump_data_size;
static int dump_overrun;
static int dump_overrun_size;
static int dump_seq_no;
static void *dump_buffer;

#define OPFLAG_OVERWRITE	0x1
static unsigned int options_flag;

#define DEVICE_NAME		"met_tag"

/* #define ERRF_ENABLE */
/* #define DEBF_ENABLE */

#ifdef ERRF_ENABLE
#define MSG_ERR			"Error:["DEVICE_NAME"]"
#define ERRF(args...)	pr_debug(MSG_ERR args)
#else
#define ERRF(args...)
#endif

#ifdef DEBF_ENABLE
#define MSG_IFO			"Info :["DEVICE_NAME"]"
#define DEBF(args...)	pr_debug(MSG_IFO args)
#else
#define DEBF(args...)
#endif

static int is_enabled(unsigned int class_id)
{
	int i;

	if (bltab.flag == 0)
		return 1;
	if (bltab.flag & MET_CLASS_ALL)
		return 0;

	for (i = 0; i < MAX_EVENT_CLASS; i++) {
		if ((bltab.flag & (1 << i)) && (bltab.class_id[i] == class_id))
			return 0;
	}

	return 1;
}

noinline int tracing_mark_write(int type, unsigned int class_id,
				       const char *name, unsigned int value,
				       unsigned int value2, unsigned int value3)
{
	if (type == TYPE_MET_SUSPEND) {
		MET_PRINTK("C|0|MET_SUSPEND|1");
		return 0;
	}
	if (type == TYPE_MET_RESUME) {
		MET_PRINTK("C|0|MET_SUSPEND|0");
		return 0;
	}
	if (!is_enabled(class_id))
		return 0;
	switch (type) {
	case TYPE_START:
		MET_PRINTK("B|%d|%s\n", class_id, name);
		break;
	case TYPE_END:
		MET_PRINTK("E|%s\n", name);
		break;
	case TYPE_ONESHOT:
		MET_PRINTK("C|%d|%s|%d\n", class_id, name, value);
		break;
	case TYPE_ASYNC_START:
		MET_PRINTK("S|%d|%s|%d\n", class_id, name, value);
		break;
	case TYPE_ASYNC_END:
		MET_PRINTK("F|%d|%s|%d\n", class_id, name, value);
		break;
	case TYPE_DUMP:
		MET_PRINTK("D|%d|%s|%d|%d|%d\n", class_id, name, value, value2, value3);
		break;
	default:
		return -1;
	}
	return 0;
}

int met_tag_init(void)
{
	memset(&bltab, 0, sizeof(struct bltable_t));
	bltab.flag = MET_CLASS_ALL;
	mutex_init(&bltab.mlock);
	return 0;
}

int met_tag_uninit(void)
{
	met_set_dump_buffer_real(0);
	return 0;
}

int met_tag_start_real(unsigned int class_id, const char *name)
{
	int ret;

	ret = tracing_mark_write(TYPE_START, class_id, name, 0, 0, 0);
#if 0
	if ((met_switch.mode & MT_SWITCH_TAGPOLLING)) {
		/* tag polling only enable when MT_SWITCH_TAGPOLLING is config */
		force_sample(NULL);
	}
#endif
	return ret;
}
EXPORT_SYMBOL(met_tag_start_real);

int met_tag_end_real(unsigned int class_id, const char *name)
{
	int ret;
#if 0
	if ((met_switch.mode & MT_SWITCH_TAGPOLLING)) {
		/* tag polling only enable when MT_SWITCH_TAGPOLLING is config */
		force_sample(NULL);
	}
#endif
	ret = tracing_mark_write(TYPE_END, class_id, name, 0, 0, 0);

	return ret;
}
EXPORT_SYMBOL(met_tag_end_real);

int met_tag_async_start_real(unsigned int class_id, const char *name, unsigned int cookie)
{
	int ret;

	ret = tracing_mark_write(TYPE_ASYNC_START, class_id, name, cookie, 0, 0);
#if 0
	if ((met_switch.mode & MT_SWITCH_TAGPOLLING)) {
		/* tag polling only enable when MT_SWITCH_TAGPOLLING is config */
		force_sample(NULL);
	}
#endif
	return ret;
}

int met_tag_async_end_real(unsigned int class_id, const char *name, unsigned int cookie)
{
	int ret;

#if 0
	if ((met_switch.mode & MT_SWITCH_TAGPOLLING)) {
		/* tag polling only enable when MT_SWITCH_TAGPOLLING is config */
		force_sample(NULL);
	}
#endif
	ret = tracing_mark_write(TYPE_ASYNC_END, class_id, name, cookie, 0, 0);
	return ret;
}

int met_tag_oneshot_real(unsigned int class_id, const char *name, unsigned int value)
{
	int ret;

	ret = tracing_mark_write(TYPE_ONESHOT, class_id, name, value, 0, 0);
#if 0
	if ((met_switch.mode & MT_SWITCH_TAGPOLLING)) {
		/* tag polling only enable when MT_SWITCH_TAGPOLLING is config */
		force_sample(NULL);
	}
#endif
	return ret;
}
EXPORT_SYMBOL(met_tag_oneshot_real);

int met_tag_userdata_real(char *pData)
{
	MET_PRINTK("%s\n", pData);
	return 0;
}

int met_tag_dump_real(unsigned int class_id, const char *name, void *data, unsigned int length)
{
	int ret;

	if ((dump_data_size + length + sizeof(int)) > dump_buffer_size) {
		if (options_flag & OPFLAG_OVERWRITE) {
			dump_overrun_size = dump_data_size;
			dump_overrun++;
			memcpy(dump_buffer, &dump_seq_no, sizeof(int));
			memcpy(dump_buffer + sizeof(int), data, length);
			ret = tracing_mark_write(TYPE_DUMP, class_id, name,
						 dump_seq_no++, 0, length + sizeof(int));
			dump_data_size = length + sizeof(int);
		} else {
			ret = tracing_mark_write(TYPE_DUMP, class_id, name, dump_seq_no++, 0, 0);
		}
	} else {
		memcpy(dump_buffer + dump_data_size, &dump_seq_no, sizeof(int));
		memcpy(dump_buffer + dump_data_size + sizeof(int), data, length);
		ret = tracing_mark_write(TYPE_DUMP, class_id, name,
					 dump_seq_no++, dump_data_size, length + sizeof(int));
		dump_data_size += length + sizeof(int);
	}
	return ret;
}

int met_tag_disable_real(unsigned int class_id)
{
	int i;

	mutex_lock(&bltab.mlock);

	if (class_id == MET_CLASS_ALL) {
		bltab.flag |= MET_CLASS_ALL;
		mutex_unlock(&bltab.mlock);
		return 0;
	}

	for (i = 0; i < MAX_EVENT_CLASS; i++) {
		if ((bltab.flag & (1 << i)) == 0) {
			bltab.class_id[i] = class_id;
			bltab.flag |= (1 << i);
			mutex_unlock(&bltab.mlock);
			return 0;
		}
	}

	mutex_unlock(&bltab.mlock);
	return -1;
}

int met_tag_enable_real(unsigned int class_id)
{
	int i;

	mutex_lock(&bltab.mlock);

	if (class_id == MET_CLASS_ALL) {
		bltab.flag &= (~MET_CLASS_ALL);
		mutex_unlock(&bltab.mlock);
		return 0;
	}

	for (i = 0; i < MAX_EVENT_CLASS; i++) {
		if ((bltab.flag & (1 << i)) && (bltab.class_id[i] == class_id)) {
			bltab.flag &= (~(1 << i));
			bltab.class_id[i] = 0;
			mutex_unlock(&bltab.mlock);
			return 0;
		}
	}

	mutex_unlock(&bltab.mlock);
	return -1;
}

int met_set_dump_buffer_real(int size)
{
	if (dump_buffer_size && dump_buffer) {
		free_pages((unsigned long)dump_buffer, get_order(dump_buffer_size));
		dump_data_size = 0;
		dump_overrun = 0;
		dump_overrun_size = 0;
		dump_seq_no = 0;
		dump_buffer_size = 0;
	}
	/* size is 0 means free dump buffer */
	if (size == 0)
		return 0;
	if (size < 0)
		return -1;

	size = (size + (PAGE_SIZE - 1)) & (~(PAGE_SIZE - 1));
	dump_buffer = (void *)__get_free_pages(GFP_KERNEL, get_order(size));
	if (dump_buffer == NULL) {
		ERRF("can not allocate buffer to copy\n");
		return -ENOMEM;
	}

	dump_buffer_size = size;
	return dump_buffer_size;
}

int met_save_dump_buffer_real(const char *pathname)
{
	int size, ret = 0;
	struct file *outfp = NULL;
	mm_segment_t oldfs;

	if (dump_data_size == 0)
		return 0;
	if (dump_data_size < 0 || dump_overrun_size < 0)
		return -1;
	if(dump_buffer == NULL || dump_buffer_size<=0)
		return -1;

	outfp = filp_open(pathname, O_WRONLY | O_TRUNC | O_CREAT, 0664);
	if (unlikely(outfp == NULL)) {
		ERRF("can not open saved file for write\n");
		return -EIO;
	}

	oldfs = get_fs();
	set_fs(KERNEL_DS);

	if (dump_overrun)
		size = dump_overrun_size;
	else
		size = dump_data_size;

	if (size >= dump_buffer_size)
		return -1;

	oldfs = get_fs();
	set_fs(KERNEL_DS);
	outfp = filp_open(pathname, O_WRONLY | O_TRUNC | O_CREAT, 0664);
	if (IS_ERR(outfp)) {
		ERRF("can not open saved file for write\n");
		return -EIO;
	}
	/* DEBF("outfp[%p] dump_buffer[%p] size(%d) outfp->f_pos(%d)\n",outfp,dump_buffer,size,outfp->f_pos); */

	ret = vfs_write(outfp, dump_buffer, size, &(outfp->f_pos));
	if (ret < 0) {
		ERRF("can not write to dump file\n");
	} else {
		dump_data_size = 0;
		dump_overrun = 0;
		dump_overrun_size = 0;
		dump_seq_no = 0;
	}

	set_fs(oldfs);
	if (outfp != NULL)
		filp_close(outfp, NULL);

	return 0;
}

int met_save_log_real(const char *pathname)
{
	int len, ret = 0;
	struct file *infp = NULL;
	struct file *outfp = NULL;
	void *ptr = NULL;
	mm_segment_t oldfs;

	infp = filp_open("/sys/kernel/debug/tracing/trace", O_RDONLY, 0);
	if (unlikely(infp == NULL)) {
		ERRF("can not open trace file for read\n");
		ret = -1;
		goto save_out;
	}

	outfp = filp_open(pathname, O_WRONLY | O_TRUNC | O_CREAT, 0664);
	if (unlikely(outfp == NULL)) {
		ERRF("can not open saved file for write\n");
		ret = -2;
		goto save_out;
	}

	ptr = (void *)__get_free_pages(GFP_KERNEL, 2);
	if (ptr == NULL) {
		ERRF("can not allocate buffer to copy\n");
		ret = -3;
		goto save_out;
	}

	oldfs = get_fs();
	set_fs(KERNEL_DS);

	while (1) {
		len = vfs_read(infp, ptr, PAGE_SIZE << 2, &(infp->f_pos));
		if (len < 0) {
			ERRF("can not read from trace file\n");
			ret = -3;
			break;
		} else if (len == 0) {
			break;
		}

		ret = vfs_write(outfp, ptr, len, &(outfp->f_pos));
		if (ret < 0) {
			ERRF("can not write to saved file\n");
			break;
		}
	}

	set_fs(oldfs);

save_out:
	if (ptr != NULL)
		free_pages((unsigned long)ptr, 2);
	if (infp != NULL)
		filp_close(infp, NULL);
	if (outfp != NULL)
		filp_close(outfp, NULL);

	return ret;
}

#ifdef BUILD_WITH_MET

#include <linux/module.h>
/* #include <asm/uaccess.h> */
#include <linux/uaccess.h>

static long met_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	/* struct mtag_cmd_t cblk; */
	struct mtag_cmd_t *pUserCmd;
	int ret = 0;
	char *str = NULL;
	int arg_int = 0;

	DEBF("mtag_ioctl cmd=%X, arg=%lX\n", cmd, arg);

	if (_IOC_TYPE(cmd) != MTAG_IOC_MAGIC)
		return -ENOTTY;

	/* Hanlde command without copying data from user space */
	if (cmd == MTAG_CMD_ENABLE) {
		if (copy_from_user(&arg_int, (char __user *)arg, sizeof(int)) )
			return -EFAULT;

		return met_tag_enable_real((unsigned int)arg_int);
	}
	else if (cmd == MTAG_CMD_DISABLE) {
		if (copy_from_user(&arg_int, (char __user *)arg, sizeof(int)) )
			return -EFAULT;

		return met_tag_disable_real((unsigned int)arg_int);
	}
	else if (cmd == MTAG_CMD_REC_SET) {
		if (copy_from_user(&arg_int, (char __user *)arg, sizeof(int)) )
			return -EFAULT;

		if (arg_int)
			tracing_on();
		else
			tracing_off();
		return 0;
	} else if (cmd == MTAG_CMD_DUMP_SIZE) {
		if (copy_from_user(&arg_int, (char __user *)arg, sizeof(int)) )
			return -EFAULT;
		return (long)ret;
	}
	/* Handle commands with user space data */
	/* Handle commands with user space data */
#if 0
	if (_IOC_DIR(cmd) & _IOC_WRITE)
		ret = !access_ok(VERIFY_READ, (void __user *)arg, _IOC_SIZE(cmd));
	if (ret)
		return -EFAULT;
#endif
	/* pUserCmd = (struct mtag_cmd_t *) arg; */
	pUserCmd = kmalloc(sizeof(struct mtag_cmd_t), GFP_KERNEL);

	if (copy_from_user(pUserCmd, (char __user *)arg, sizeof(struct mtag_cmd_t))) {
		ret = -EFAULT;
		goto exit;
	}

	/* DEBF("class_id(%d) size(%d)\n",pUserCmd->class_id,pUserCmd->size); */
	/* prevent the value err */
	pUserCmd->tname[MAX_TAGNAME_LEN-1]='\0';

#if 0
	__get_user(cblk.class_id, (unsigned int __user *)(&(pUserCmd->class_id)));
	__get_user(cblk.value, (unsigned int __user *)(&(pUserCmd->value)));
	__get_user(cblk.slen, (unsigned int __user *)(&(pUserCmd->slen)));
	ret = __copy_from_user(cblk.tname, (char __user *)(&(pUserCmd->tname)), cblk.slen);
	if (unlikely(ret)) {
		ERRF("Failed to __copy_from_user: ret=%d\n", ret);
		return -EFAULT;
	}

	switch (cmd) {
	case MTAG_CMD_START:
		ret = met_tag_start_real(cblk.class_id, (char *)cblk.tname);
		break;
	case MTAG_CMD_END:
		ret = met_tag_end_real(cblk.class_id, (char *)cblk.tname);
		break;
	case MTAG_CMD_ONESHOT:
		ret = met_tag_oneshot_real(cblk.class_id, (char *)cblk.tname, cblk.value);
		break;
	default:
		return -EINVAL;
	}
#else
	switch (cmd) {
	case MTAG_CMD_START:
		ret = met_tag_start_real(pUserCmd->class_id, pUserCmd->tname);
		break;
	case MTAG_CMD_END:
		ret = met_tag_end_real(pUserCmd->class_id, pUserCmd->tname);
		break;
	case MTAG_CMD_ASYNC_START:
		ret = met_tag_async_start_real(pUserCmd->class_id, pUserCmd->tname, pUserCmd->value);
		break;
	case MTAG_CMD_ASYNC_END:
		ret = met_tag_async_end_real(pUserCmd->class_id, pUserCmd->tname, pUserCmd->value);
		break;
	case MTAG_CMD_ONESHOT:
		ret = met_tag_oneshot_real(pUserCmd->class_id, pUserCmd->tname, pUserCmd->value);
		break;
	case MTAG_CMD_DUMP:
		ret = !access_ok(VERIFY_READ, (void __user *)pUserCmd->data, pUserCmd->size);
		if (ret)
			return -EFAULT;
		ret =
		    met_tag_dump_real(pUserCmd->class_id, pUserCmd->tname, pUserCmd->data,
				      pUserCmd->size);
		break;
	case MTAG_CMD_DUMP_SAVE:
		ret = met_save_dump_buffer_real(pUserCmd->tname);
		break;
	case MTAG_CMD_USRDATA:
		str = kmalloc(pUserCmd->slen, GFP_KERNEL);
		if (!str) {
			ret = -ENOMEM;
			goto exit;
		}
		if (copy_from_user(str, (void __user *)pUserCmd->data, pUserCmd->slen)) {
			ret = -EFAULT;
			kfree(str);
			goto exit;
		}
		met_tag_userdata_real(str);
		kfree(str);
		goto exit;
		/* break; */
	default:
		kfree(pUserCmd);
		return -EINVAL;
	}
#endif
exit:
	kfree(pUserCmd);
	return ret;
}

static ssize_t met_write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos)
{
	DEBF(MSG_IFO "mtag_write buf=%p size=%d\n", buf, (int)count);
	return 0;
}

static ssize_t met_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos)
{
	DEBF(MSG_IFO "mtag_read buf=%p size=%d\n", buf, (int)count);
	return 0;
}

static int met_open(struct inode *inode, struct file *filp)
{
	DEBF(MSG_IFO "mtag_open successfully\n");
	return 0;
}

static int met_close(struct inode *inode, struct file *filp)
{
	DEBF(MSG_IFO "mtag_close successfully\n");
	return 0;
}

/* =========================================================================== */
/* misc file nodes */
/* =========================================================================== */
static ssize_t enable_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
	int i;

	i = snprintf(buf, PAGE_SIZE, "%d\n", (bltab.flag >> 31) ? 0 : 1);
	return i;
}

static ssize_t enable_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf,
			    size_t n)
{
	int value;

	if ((n == 0) || (buf == NULL))
		return -EINVAL;
	if (kstrtoint(buf, 0, &value) != 0)
		return -EINVAL;

	mutex_lock(&bltab.mlock);

	if (value == 1)
		bltab.flag &= (~MET_CLASS_ALL);
	else
		bltab.flag |= MET_CLASS_ALL;

	mutex_unlock(&bltab.mlock);

	return n;
}

static struct kobject *kobj_tag;
static struct kobj_attribute enable_attr = __ATTR(enable, 0664, enable_show, enable_store);

static ssize_t dump_buffer_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
	int i, size;

	if (dump_overrun)
		size = dump_overrun_size;
	else
		size = dump_data_size;

	i = snprintf(buf, PAGE_SIZE, "Buffer Size (KB)=%d\nData Size (KB)=%d\nOverrun=%d\n",
		     dump_buffer_size >> 10, size >> 10, dump_overrun);
	return i;
}

static ssize_t dump_buffer_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf,
				 size_t n)
{
	int ret, value;

	if ((n == 0) || (buf == NULL))
		return -EINVAL;
	if (kstrtoint(buf, 0, &value) != 0)
		return -EINVAL;

	ret = met_set_dump_buffer_real(value << 10);

	if (ret < 0)
		return ret;

	return n;
}

static struct kobj_attribute dump_buffer_attr =
__ATTR(dump_buffer_kb, 0664, dump_buffer_show, dump_buffer_store);

static ssize_t options_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
	int i = 0;

	buf[0] = 0;

	if (options_flag == 0) {
		strncat(buf, "none\n", PAGE_SIZE - 1 - i);
		i += 5;
	}

	if (options_flag & OPFLAG_OVERWRITE) {
		strncat(buf, "overwrite\n", PAGE_SIZE - 1 - i);
		i += 10;
	}

	return i;
}

static ssize_t options_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf,
			     size_t n)
{
	if ((n == 0) || (buf == NULL))
		return -EINVAL;

	if ((n == 1) && (buf[0] == 0xA)) {
		options_flag = 0;
		return n;
	}

	if (strncmp(buf, "overwrite", 9) == 0)
		options_flag |= OPFLAG_OVERWRITE;
	else
		return -EINVAL;

	return n;
}

static struct kobj_attribute options_attr = __ATTR(options, 0664, options_show, options_store);

int tag_reg(struct file_operations *const fops, struct kobject *kobj)
{
	int ret;

	kobj_tag = kobject_create_and_add("tag", kobj);
	if (kobj_tag == NULL) {
		ERRF("can not create kobject: kobj_bus\n");
		return -1;
	}

	ret = sysfs_create_file(kobj_tag, &enable_attr.attr);
	if (ret != 0) {
		ERRF("Failed to create enable in sysfs\n");
		kobject_del(kobj_tag);
		kobject_put(kobj_tag);
		kobj_tag = NULL;
		return ret;
	}

	ret = sysfs_create_file(kobj_tag, &dump_buffer_attr.attr);
	if (ret != 0) {
		ERRF("Failed to create dump_buffer in sysfs\n");
		sysfs_remove_file(kobj_tag, &enable_attr.attr);
		kobject_del(kobj_tag);
		kobject_put(kobj_tag);
		kobj_tag = NULL;
		return ret;
	}

	ret = sysfs_create_file(kobj_tag, &options_attr.attr);
	if (ret != 0) {
		ERRF("Failed to create options in sysfs\n");
		sysfs_remove_file(kobj_tag, &enable_attr.attr);
		sysfs_remove_file(kobj_tag, &dump_buffer_attr.attr);
		kobject_del(kobj_tag);
		kobject_put(kobj_tag);
		kobj_tag = NULL;
		return ret;
	}

	fops->open = met_open;
	fops->release = met_close;
	fops->unlocked_ioctl = met_ioctl;
	fops->read = met_read;
	fops->write = met_write;
	met_tag_init();
	met_ext_api.met_tag_start = met_tag_start_real;
	met_ext_api.met_tag_end = met_tag_end_real;
	met_ext_api.met_tag_async_start = met_tag_async_start_real;
	met_ext_api.met_tag_async_end = met_tag_async_end_real;
	met_ext_api.met_tag_oneshot = met_tag_oneshot_real;
	met_ext_api.met_tag_userdata = met_tag_userdata_real;
	met_ext_api.met_tag_dump = met_tag_dump_real;
	met_ext_api.met_tag_disable = met_tag_disable_real;
	met_ext_api.met_tag_enable = met_tag_enable_real;
	met_ext_api.met_set_dump_buffer = met_set_dump_buffer_real;
	met_ext_api.met_save_dump_buffer = met_save_dump_buffer_real;
	met_ext_api.met_save_log = met_save_log_real;
	met_ext_api.met_sched_switch = met_sched_switch;
	return 0;
}

int tag_unreg(void)
{
	met_ext_api.met_tag_start = NULL;
	met_ext_api.met_tag_end = NULL;
	met_ext_api.met_tag_async_start = NULL;
	met_ext_api.met_tag_async_end = NULL;
	met_ext_api.met_tag_oneshot = NULL;
	met_ext_api.met_tag_userdata = NULL;
	met_ext_api.met_tag_dump = NULL;
	met_ext_api.met_tag_disable = NULL;
	met_ext_api.met_tag_enable = NULL;
	met_ext_api.met_set_dump_buffer = NULL;
	met_ext_api.met_save_dump_buffer = NULL;
	met_ext_api.met_save_log = NULL;
	met_ext_api.met_show_bw_limiter = NULL;
	met_ext_api.met_reg_bw_limiter = NULL;
	met_ext_api.met_show_clk_tree = NULL;
	met_ext_api.met_reg_clk_tree = NULL;
	met_ext_api.met_sched_switch = NULL;
	met_tag_uninit();
	sysfs_remove_file(kobj_tag, &enable_attr.attr);
	sysfs_remove_file(kobj_tag, &dump_buffer_attr.attr);
	sysfs_remove_file(kobj_tag, &options_attr.attr);
	kobject_del(kobj_tag);
	kobject_put(kobj_tag);
	kobj_tag = NULL;
	return 0;
}

#endif				/* BUILD_WITH_MET */

#else				/* not MET_USER_EVENT_SUPPORT */

#ifdef BUILD_WITH_MET
int tag_reg(void *p, void *q)
{
	return 0;
}

int tag_unreg(void)
{
	return 0;
}
#endif

#endif				/* MET_USER_EVENT_SUPPORT */
