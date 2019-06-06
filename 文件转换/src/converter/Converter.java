package converter;

import javax.imageio.ImageIO;
import java.awt.image.BufferedImage;
import java.io.File;
import java.io.FileOutputStream;

public class Converter {

    public static final File baseFile = new File("C:\\Users\\Kuresaru\\Desktop\\新建文件夹\\32");

    public static void main(String[] args) throws Exception {
        File[] files = baseFile.listFiles();
//        FileWriter writer = new FileWriter(new File(baseFile, "01.bin"), false);
        FileOutputStream outputStream = new FileOutputStream(new File(baseFile, "01.bin"), false);
        for (File file : files) {
            BufferedImage image = ImageIO.read(file);
            for (int y = 0; y < 32; y++) {
                for (int x = 0; x < 32; x++) {
                    int color = image.getRGB(x, y);
                    outputStream.write((byte) 0x00);
                    outputStream.write((byte) ((color >> 16) & 0xFF));
                    outputStream.write((byte) ((color >> 8) & 0xFF));
                    outputStream.write((byte) (color & 0xFF));
                    outputStream.flush();
                }
            }
        }
        outputStream.close();
    }

}
