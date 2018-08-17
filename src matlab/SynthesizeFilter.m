% function synthesizes a filter
% OutImg is filter mask
% pos - %4-element vector of the form [xmin ymin width height]. The initial size of the rectangle is width-by-height pixels. The upper-left corner of the rectangle is at the (x,y) coordinate (xmin,ymin).
function OutImg = SynthesizeFilter(h, w, pos)

OutImg = ones([h w]);

xmin = pos(1);
ymin = pos(2);
width = pos(3);
height = pos(4);

OutImg(ymin:ymin+height, xmin:xmin+width) = 0;

ymin2 = h - pos(2) - height+3;
OutImg(ymin2:ymin2+height, xmin:xmin+width) = 0;

xmin2 = w - pos(1) - width+3;
OutImg(ymin:ymin+height, xmin2:xmin2+width) = 0;
OutImg(ymin2:ymin2+height, xmin2:xmin2+width) = 0;