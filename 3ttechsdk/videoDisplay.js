'use strict'

/**
 * video display are uitities for dispplay image on canvas.
 */
class videoDisplay {
    constructor() {
    }

    yuv2canvas(yuvData, width, height, canvas) {
        /*
        canvas.width = width;
        canvas.height = height;
        */
        let context = canvas.getContext("2d");
        let imageData = context.createImageData(width, height);
        let yuv = Buffer.from(yuvData, 'base64');

        let yOffset = 0;
        let uOffset = width * height;
        let vOffset = width * height + (width * height) / 4;
        for (var h = 0; h < height; h++) {
            for (var w = 0; w < width; w++) {
                let ypos = w + h * width + yOffset;

                let upos = (w >> 1) + (h >> 1) * width / 2 + uOffset;
                let vpos = (w >> 1) + (h >> 1) * width / 2 + vOffset;

                let Y = yuv[ypos];
                let U = yuv[upos] - 128;
                let V = yuv[vpos] - 128;

                let R = (Y + 1.371 * V);
                let G = (Y - 0.698 * V - 0.336 * U);
                let B = (Y + 1.732 * U);

                let outputData_pos = w * 4 + width * h * 4;
                imageData.data[0 + outputData_pos] = R;
                imageData.data[1 + outputData_pos] = G;
                imageData.data[2 + outputData_pos] = B;
                imageData.data[3 + outputData_pos] = 255;
            }
        }

        context.putImageData(imageData, 0, 0);
        // let img = this.imagedata_to_image(imageData, canvas);
        // context.drawImage(img, 0, 0, 300, 300);
    }

    rgba2Canvas(rgba, width, height, canvas) {
        let context = canvas.getContext("2d");
        let imageData = context.createImageData(width, height);
        let buf = Buffer.from(rgba, 'base64');
        buf.copy(imageData.data);
        context.putImageData(imageData, 0, 0);
    }

    imagedata_to_image(imagedata, canvas) {
        var ctx = canvas.getContext('2d');
        canvas.width = imagedata.width;
        canvas.height = imagedata.height;
        ctx.putImageData(imagedata, 0, 0);
    
        var image = new Image();
        image.src = canvas.toDataURL();
        return image;
    }
}

module.exports = videoDisplay;