from PIL import Image
import pyperclip

def get_pixel(img, x, y, val = (0, 0, 0)):
    r, g, b = img.getpixel((x, y))[:3]
    
    if (r, g, b) == val:
        return True
    else:
        return False

def process_png(image_name):
    img = Image.open(image_name + ".png")
    
    output_text = ""
    output_text += "{\n"

    for y in range(img.height):
        output_text += "    "
        for x in range(img.width):
            
            if get_pixel(img, x, y, (255, 255, 255)) or get_pixel(img, x, y, (255, 0, 0)):
                output_text += "1, "
            else:
                output_text += "0, "

        output_text += "//\n"

    output_text += "}\n"

    pyperclip.copy(output_text)
    print("Wygenerowane dane zostały skopiowane do schowka!")
    
def generate_high_res(image_name):
    base_img = Image.open(image_name + ".png")
    lookup_img = Image.open("cropLookup.png")
    target = Image.new("RGB", (base_img.width * 40, base_img.height * 40), (255, 255, 255))

    for y in range(base_img.height):
        for x in range(base_img.width):
            lookup_x = 0
            lookup_y = 0

            left = max(x - 1, 0)
            right = min(x + 1, base_img.width - 1)
            up = max(y - 1, 0)
            down = min(y + 1, base_img.height - 1)

            thisPixel = get_pixel(base_img, x, y, (255, 0, 0))
            leftPixel = get_pixel(base_img, left, y, (255, 255, 255)) or get_pixel(base_img, left, y, (255, 0, 0))
            rightPixel = get_pixel(base_img, right, y, (255, 255, 255)) or get_pixel(base_img, right, y, (255, 0, 0))
            upPixel = get_pixel(base_img, x, up, (255, 255, 255)) or get_pixel(base_img, x, up, (255, 0, 0))
            downPixel = get_pixel(base_img, x, down, (255, 255, 255)) or get_pixel(base_img, x, down, (255, 0, 0))

            if (thisPixel):

                if (rightPixel
                and downPixel
                ):
                    lookup_x = 6
                    lookup_y = 0

                if (leftPixel
                and downPixel
                ):
                    lookup_x = 0
                    lookup_y = 1

                if (rightPixel
                and upPixel
                ):
                    lookup_x = 1
                    lookup_y = 1

                if (leftPixel
                and upPixel
                ):
                    lookup_x = 2
                    lookup_y = 1

            for sy in range(40):
                for sx in range(40):
                    target.putpixel((x * 40 + sx, y * 40 + sy), lookup_img.getpixel((lookup_x * 40 + sx, lookup_y * 40 + sy))[:3])

    for y in range(base_img.height):
        for x in range(base_img.width):

            lookup_x = 0
            lookup_y = 0

            left = max(x - 1, 0)
            right = min(x + 1, base_img.width - 1)
            up = max(y - 1, 0)
            down = min(y + 1, base_img.height - 1)

            thisPixel = get_pixel(base_img, x, y, (0, 0, 0))

            leftPixel = get_pixel(base_img, left, y, (255, 255, 255)) or get_pixel(base_img, left, y, (255, 0, 0))
            rightPixel = get_pixel(base_img, right, y, (255, 255, 255)) or get_pixel(base_img, right, y, (255, 0, 0))
            upPixel = get_pixel(base_img, x, up, (255, 255, 255)) or get_pixel(base_img, x, up, (255, 0, 0))
            downPixel = get_pixel(base_img, x, down, (255, 255, 255)) or get_pixel(base_img, x, down, (255, 0, 0))

            leftUpPixel = get_pixel(base_img, left, up, (255, 255, 255)) or get_pixel(base_img, left, up, (255, 0, 0))
            rightUpPixel = get_pixel(base_img, right, up, (255, 255, 255)) or get_pixel(base_img, right, up, (255, 0, 0))
            leftDownPixel = get_pixel(base_img, left, down, (255, 255, 255)) or get_pixel(base_img, left, down, (255, 0, 0))
            rightDownPixel = get_pixel(base_img, right, down, (255, 255, 255)) or get_pixel(base_img, right, down, (255, 0, 0))

            if thisPixel:

                if (rightPixel
                and downPixel
                and rightDownPixel
                ):
                    lookup_x = 3
                    lookup_y = 1

                if (leftPixel
                and downPixel
                and leftDownPixel
                ):
                    lookup_x = 4
                    lookup_y = 1

                if (rightPixel
                and upPixel
                and rightUpPixel
                ):
                    lookup_x = 5
                    lookup_y = 1

                if (leftPixel
                and upPixel
                and leftUpPixel
                ):
                    lookup_x = 6
                    lookup_y = 1

                for sy in range(40):
                    for sx in range(40):
                        target.putpixel((x * 40 + sx, y * 40 + sy), lookup_img.getpixel((lookup_x * 40 + sx, lookup_y * 40 + sy))[:3])

    for y in range(base_img.height):
        for x in range(base_img.width):
            lookup_x = 0
            lookup_y = 0

            left = max(x - 1, 0)
            right = min(x + 1, base_img.width - 1)
            up = max(y - 1, 0)
            down = min(y + 1, base_img.height - 1)

            thisPixel = get_pixel(base_img, x, y, (255, 255, 255))
            leftPixel = get_pixel(base_img, left, y, (255, 255, 255)) or get_pixel(base_img, left, y, (255, 0, 0))
            rightPixel = get_pixel(base_img, right, y, (255, 255, 255)) or get_pixel(base_img, right, y, (255, 0, 0))
            upPixel = get_pixel(base_img, x, up, (255, 255, 255)) or get_pixel(base_img, x, up, (255, 0, 0))
            downPixel = get_pixel(base_img, x, down, (255, 255, 255)) or get_pixel(base_img, x, down, (255, 0, 0))
                
            if (thisPixel):

                if (not upPixel
                and downPixel
                ):
                    lookup_x = 2
                    lookup_y = 0

                if (upPixel
                and not downPixel
                ):
                    lookup_x = 3
                    lookup_y = 0

                if (not leftPixel
                and rightPixel
                ):
                    lookup_x = 4
                    lookup_y = 0

                if (leftPixel
                and not rightPixel
                ):
                    lookup_x = 5
                    lookup_y = 0

                if ((leftPixel and rightPixel) or (upPixel and downPixel)
                ):
                    lookup_x = 1
                    lookup_y = 0

                for sy in range(40):
                    for sx in range(40):
                        target.putpixel((x * 40 + sx, y * 40 + sy), lookup_img.getpixel((lookup_x * 40 + sx, lookup_y * 40 + sy))[:3])

    target = target.convert("RGBA")

    for y in range(target.height):
        for x in range(target.width):
            
            thisPixel = get_pixel(target, x, y, (0, 0, 0))
                
            if (thisPixel):

                target.putpixel((x, y), (0, 0, 0, 0))
                        

    target.save(image_name + "_stamped.png")

name = input()
process_png(name)
generate_high_res(name)
