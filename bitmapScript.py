from PIL import Image


def image_to_bitmap(image_path):
    """
    Transforms an image (128x64 pixels) into a C-style bitmap array.

    Args:
        image_path (str): Path to the input image.

    Returns:
        str: The C-style bitmap array as a string.
    """
    img = Image.open(image_path).convert("1")  
    img = img.resize((128, 64))

    pixels = img.getdata()
    width, height = img.size

    bitmap = []
    for y in range(height):
        row = 0
        for x in range(width):
            pixel = pixels[y * width + x]
            row = (row << 1) | (0 if pixel == 0 else 1)
            if (x + 1) % 8 == 0: 
                bitmap.append(row)
                row = 0
        if width % 8 != 0:  
            row <<= 8 - (width % 8)
            bitmap.append(row)

    formatted_bitmap = ", ".join(f"0x{byte:02X}" for byte in bitmap)
    c_array = f"const unsigned char bitmap[] = {{\n{formatted_bitmap}\n}};"

    return c_array


image_path = "input_image.png"  
bitmap_code = image_to_bitmap(image_path)

with open("output_bitmap.h", "w") as f:
    f.write(bitmap_code)

print("Bitmap generated and saved to output_bitmap.h")
