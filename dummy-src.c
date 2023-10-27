/*
 * Art Spa
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/module.h>
#include <linux/of.h>
#include <linux/platform_device.h>
#include <linux/slab.h>
//#include <linux/xilinx-hls.h>
//#include <linux/xilinx-v4l2-controls.h>





#include <media/v4l2-async.h>
#include <media/v4l2-ctrls.h>
#include <media/v4l2-subdev.h>

//#include "xilinx-video-axis-src.h"
//#include "xilinx-vip.h"





/**
 * struct xaxis_src_device - Xilinx HLS Core device structure
 * @xvip: Xilinx Video IP device
 * @pads: media pads
 * @compatible: first DT compatible string for the device
 * @formats: active V4L2 media bus formats at the sink and source pads
 * @default_formats: default V4L2 media bus formats
 * @vip_formats: format information corresponding to the pads active formats
 * @model: additional description of IP implementation if available
 * @ctrl_handler: control handler
 * @user_mem: user portion of the register space
 * @user_mem_size: size of the user portion of the register space
 */
struct device
{
        /* data */
};

struct xaxis_src_device {
        struct device xvip;
        struct media_pad pads[2];

        const char *compatible;

        struct v4l2_mbus_framefmt formats[2];
        struct v4l2_mbus_framefmt default_formats;
        const struct xvip_video_format *vip_formats;

        struct v4l2_ctrl_handler ctrl_handler;
        struct v4l2_ctrl *model;

        int src_frame_width;
        int src_frame_height;
};

static inline struct xaxis_src_device *to_axis_src(struct v4l2_subdev *subdev)
{
        return container_of(subdev, struct xaxis_src_device, xvip.subdev);
}
/* -----------------------------------------------------------------------------
 * Controls
 */

static const struct v4l2_ctrl_config xaxis_src_model_ctrl = {
        .id     = V4L2_CID_XILINX_HLS_MODEL,
        .name   = "AXIS Src Model",
        .type   = V4L2_CTRL_TYPE_STRING,
        .step   = 1,
        .flags  = V4L2_CTRL_FLAG_READ_ONLY,
};

static int xaxis_src_create_controls(struct xaxis_src_device *xaxis_src)
{
        struct v4l2_ctrl_config model = xaxis_src_model_ctrl;
        struct v4l2_ctrl *ctrl;
        int ret;

        model.max = strlen(xaxis_src->compatible);
        model.min = model.max;

        ret = v4l2_ctrl_handler_init(&xaxis_src->ctrl_handler, 1);
        if (ret) {
                dev_err(xaxis_src->xvip.dev,
                        "failed to initializing controls (%d)\n", ret);
                return ret;
        }

        ctrl = v4l2_ctrl_new_custom(&xaxis_src->ctrl_handler, &model, NULL);

        if (xaxis_src->ctrl_handler.error || !ctrl) {
                dev_err(xaxis_src->xvip.dev, "failed to add controls\n");
                v4l2_ctrl_handler_free(&xaxis_src->ctrl_handler);
                return xaxis_src->ctrl_handler.error;
        }

        v4l2_ctrl_s_ctrl_string(ctrl, xaxis_src->compatible);

   xaxis_src->xvip.subdev.ctrl_handler = &xaxis_src->ctrl_handler;

        return 0;
}

/* -----------------------------------------------------------------------------
 * V4L2 Subdevice Core Operations
 */

static long xaxis_src_ioctl(struct v4l2_subdev *subdev, unsigned int cmd, void *arg)
{
        struct xaxis_src_device *xaxis_src = to_axis_src(subdev);

        return -ENOTTY;
}

/* -----------------------------------------------------------------------------
 * V4L2 Subdevice Video Operations
 */

static int xaxis_src_s_stream(struct v4l2_subdev *subdev, int enable)
{
        //struct xaxis_src_device *xaxis_src = to_axis_src(subdev);
        //struct v4l2_mbus_framefmt *format = &xaxis_src->formats[XVIP_PAD_SINK];

        return 0;
}

/* -----------------------------------------------------------------------------
 * V4L2 Subdevice Pad Operations
 */
static struct v4l2_mbus_framefmt *
__xaxis_src_get_pad_format(struct xaxis_src_device *xaxis_src,
                      struct v4l2_subdev_state *state,
                      unsigned int pad, u32 which)
{
        struct v4l2_mbus_framefmt *format;

        switch (which) {
        case V4L2_SUBDEV_FORMAT_TRY:
                format = v4l2_subdev_get_try_format(&xaxis_src->xvip.subdev,
                                                    state, pad);
                break;
        case V4L2_SUBDEV_FORMAT_ACTIVE:
                format = &xaxis_src->formats[pad];
                break;
        default:
                format = NULL;
                break;
        }

        return format;
}
static int xaxis_src_get_format(struct v4l2_subdev *subdev,
                           struct v4l2_subdev_state *state,
                           struct v4l2_subdev_format *fmt)
{
        struct xaxis_src_device *xaxis_src = to_axis_src(subdev);
        struct v4l2_mbus_framefmt *format;

        format = __xaxis_src_get_pad_format(xaxis_src, state, fmt->pad, fmt->which);
        if (!format)
                return -EINVAL;

        fmt->format = *format;

        return 0;
}

static int xaxis_src_set_format(struct v4l2_subdev *subdev,
                           struct v4l2_subdev_state *state,
                           struct v4l2_subdev_format *fmt)
{
        struct xaxis_src_device *xaxis_src = to_axis_src(subdev);
        struct v4l2_mbus_framefmt *format;

        format = __xaxis_src_get_pad_format(xaxis_src, state, fmt->pad, fmt->which);
        if (!format)
                return -EINVAL;

        fmt->format = *format;

        xvip_set_format_size(format, fmt);

        fmt->format = *format;

        return 0;
}

/* -----------------------------------------------------------------------------
 * V4L2 Subdevice Operations
 */

static int xaxis_src_open(struct v4l2_subdev *subdev, struct v4l2_subdev_fh *fh)
{
        struct xaxis_src_device *xaxis_src = to_axis_src(subdev);
        struct v4l2_mbus_framefmt *format;

        /* Initialize with default formats */
        format = v4l2_subdev_get_try_format(subdev, fh->state, 0);
        *format = xaxis_src->default_formats;

        return 0;
}

static int xaxis_src_close(struct v4l2_subdev *subdev, struct v4l2_subdev_fh *fh)
{
        return 0;
}

static struct v4l2_subdev_core_ops xaxis_src_core_ops = {
        .ioctl = xaxis_src_ioctl,
};

static struct v4l2_subdev_video_ops xaxis_src_video_ops = {
        .s_stream = xaxis_src_s_stream,
};

static struct v4l2_subdev_pad_ops xaxis_src_pad_ops = {
        .enum_mbus_code = xvip_enum_mbus_code,
        .enum_frame_size = xvip_enum_frame_size,
        .get_fmt = xaxis_src_get_format,
        .set_fmt = xaxis_src_set_format,
};

static struct v4l2_subdev_ops xaxis_src_ops = {
        .core   = &xaxis_src_core_ops,
        .video  = &xaxis_src_video_ops,
        .pad    = &xaxis_src_pad_ops,
};

static const struct v4l2_subdev_internal_ops xaxis_src_internal_ops = {
        .open = xaxis_src_open,
        .close = xaxis_src_close,
};

/* -----------------------------------------------------------------------------
 * Media Operations
 */

static const struct media_entity_operations xaxis_src_media_ops = {
        .link_validate = v4l2_subdev_link_validate,
};

/* -----------------------------------------------------------------------------
 * Platform Device Driver
 */
/* -----------------------------------------------------------------------------
 * Platform Device Driver
 */

static void xaxis_src_init_formats(struct xaxis_src_device *xaxis_src)
{
        struct v4l2_mbus_framefmt *format;

        /* Initialize default and active formats */
        format = &xaxis_src->default_formats;
        format->code = xaxis_src->vip_formats->code;
        format->field = V4L2_FIELD_NONE;
        format->colorspace = V4L2_COLORSPACE_SRGB;

        format->width = xaxis_src->src_frame_width;
        format->height = xaxis_src->src_frame_height;

        xaxis_src->formats[0] = *format;
}

static int xaxis_src_parse_of(struct xaxis_src_device *xaxis_src)
{
        struct device *dev = xaxis_src->xvip.dev;
        struct device_node *node = xaxis_src->xvip.dev->of_node;
        struct device_node *ports;
        struct device_node *port;
        u32 port_id;
        int ret;

        ret = of_property_read_string(node, "compatible", &xaxis_src->compatible);
        if (ret < 0)
                return -EINVAL;

        ports = of_get_child_by_name(node, "ports");
        if (ports == NULL)
                ports = node;
  /* Get the format description for each pad */
        for_each_child_of_node(ports, port) {
                if (port->name && (of_node_cmp(port->name, "port") == 0)) {
                        const struct xvip_video_format *vip_format;

                        vip_format = xvip_of_get_format(port);
                        if (IS_ERR(vip_format)) {
                                dev_err(dev, "invalid format in DT");
                                return PTR_ERR(vip_format);
                        }

                        ret = of_property_read_u32(port, "reg", &port_id);
                        if (ret < 0) {
                                dev_err(dev, "no reg in DT");
                                return ret;
                        }
#if 0
                        if (port_id != 1) {
                                dev_err(dev, "invalid reg in DT");
                                return -EINVAL;
                        }
#endif
                        xaxis_src->vip_formats = vip_format;

                        ret = of_property_read_u32(port, "xlnx,frame-width", &xaxis_src->src_frame_width);
                        if (ret < 0){
                                dev_err(dev, "no video w definition in DT");
                                return -EINVAL;
                        }

                        ret = of_property_read_u32(port, "xlnx,frame-height", &xaxis_src->src_frame_height);
                        if (ret < 0){
                                dev_err(dev, "no video height definition in DT");
                                return -EINVAL;
                        }
                }
        }

        return 0;
}

static int xaxis_src_probe(struct platform_device *pdev)
{
        struct v4l2_subdev *subdev;
        struct xaxis_src_device *xaxis_src;

        int ret;


        xaxis_src = devm_kzalloc(&pdev->dev, sizeof(*xaxis_src), GFP_KERNEL);
        if (!xaxis_src)
                return -ENOMEM;

        xaxis_src->xvip.dev = &pdev->dev;

        ret = xaxis_src_parse_of(xaxis_src);
  if (ret < 0)
                return ret;
#if 0
        ret = xvip_init_resources(&xaxis_src->xvip);
        if (ret < 0)
                return ret;
#endif

        /* Initialize V4L2 subdevice and media entity */
        subdev = &xaxis_src->xvip.subdev;
        v4l2_subdev_init(subdev, &xaxis_src_ops);

        subdev->dev = &pdev->dev;
        subdev->internal_ops = &xaxis_src_internal_ops;
        strlcpy(subdev->name, dev_name(&pdev->dev), sizeof(subdev->name));
        v4l2_set_subdevdata(subdev, xaxis_src);

        subdev->flags |= V4L2_SUBDEV_FL_HAS_DEVNODE;

        xaxis_src_init_formats(xaxis_src);

        xaxis_src->pads[0].flags = MEDIA_PAD_FL_SOURCE;
        subdev->entity.ops = &xaxis_src_media_ops;
        ret = media_entity_pads_init(&subdev->entity, 1, xaxis_src->pads);

        if (ret < 0)
                goto error;
#if 1
        ret = xaxis_src_create_controls(xaxis_src);
        if (ret < 0)
                goto error;
#endif
        platform_set_drvdata(pdev, xaxis_src);

        ret = v4l2_async_register_subdev(subdev);
        if (ret < 0) {
                dev_err(&pdev->dev, "failed to register subdev\n");
                goto error;
        }

        dev_info(xaxis_src->xvip.dev, "device %s found\n", xaxis_src->compatible);

        return 0;

error:
        v4l2_ctrl_handler_free(&xaxis_src->ctrl_handler);
        media_entity_cleanup(&subdev->entity);
        xvip_cleanup_resources(&xaxis_src->xvip);
        return ret;
}

static int xaxis_src_remove(struct platform_device *pdev)
{
        struct xaxis_src_device *xaxis_src = platform_get_drvdata(pdev);
        struct v4l2_subdev *subdev = &xaxis_src->xvip.subdev;

        v4l2_async_unregister_subdev(subdev);
        v4l2_ctrl_handler_free(&xaxis_src->ctrl_handler);
        media_entity_cleanup(&subdev->entity);

        xvip_cleanup_resources(&xaxis_src->xvip);

        return 0;
}

static const struct of_device_id xaxis_src_of_id_table[] = {
        { .compatible = "xlnx,v-axis-dummy-src" },
        { }
};
MODULE_DEVICE_TABLE(of, xaxis_src_of_id_table);

static struct platform_driver xaxis_src_driver = {
        .driver = {
                .name = "xilinx-axis-dummy-src",
                .of_match_table = xaxis_src_of_id_table,
        },
        .probe = xaxis_src_probe,
        .remove = xaxis_src_remove,
};

module_platform_driver(xaxis_src_driver);

MODULE_AUTHOR("Riccardo Nuti <riccardo.nuti@artgroup-spa.com>");
MODULE_DESCRIPTION("Axis video source Driver");
MODULE_LICENSE("GPL v2");



