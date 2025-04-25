from PIL import Image
import pyperclip

def process_png(image_name):
    img = Image.open(image_name + ".png")
    
    output_text = ""
    output_text += "std::vector<bool> m_crop_pattern = {\n"

    for y in range(9):
        output_text += "    "
        for x in range(9):
            r, g, b = img.getpixel((x, y))[:3]
            
            if (r, g, b) == (0, 0, 0):
                output_text += "0, "
            else:
                output_text += "1, "

        output_text += "//\n"

    output_text += "};\n"

    pyperclip.copy(output_text)
    print("Wygenerowane dane zostały skopiowane do schowka!")

name = input()
process_png(name)
