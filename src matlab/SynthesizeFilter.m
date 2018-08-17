% function synthesizes a filter
function OutImg = SynthesizeFilter(h, w, pos)

OutImg = ones([h w]);

xmin = pos(1);
ymin = pos(2);
width = pos(3);
height = pos(4);

OutImg(ymin:ymin+height, xmin:xmin+width) = 0; %imgD(row, colomn), this is why imgD(y,x) rather than (x,y)

ymin2 = h - pos(2) - height+3;
OutImg(ymin2:ymin2+height, xmin:xmin+width) = 0; %imgD(row, colomn), this is why imgD(y,x) rather than (x,y)

xmin2 = w - pos(1) - width+3;
OutImg(ymin:ymin+height, xmin2:xmin2+width) = 0; %imgD(row, colomn), this is why imgD(y,x) rather than (x,y)
OutImg(ymin2:ymin2+height, xmin2:xmin2+width) = 0; %imgD(row, colomn), this is why imgD(y,x) rather than (x,y)
