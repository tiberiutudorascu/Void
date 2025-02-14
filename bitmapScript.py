from PIL import Image


def image_to_bitmap(image_path):
    """
    Transforms an image (128x64 pixels) into a C-style bitmap array.

    Args:
        image_path (str): Path to the input image.

    Returns:
        str: The C-style bitmap array as a string.
    """
    # Open the image
    img = Image.open(image_path).convert("1")  # Convert to black and white
    img = img.resize((128, 64))  # Ensure it's 128x64 pixels

    # Get the pixel data
    pixels = img.getdata()
    width, height = img.size

    # Convert the pixels into a C-style bitmap
    bitmap = []
    for y in range(height):
        row = 0
        for x in range(width):
            pixel = pixels[y * width + x]
            row = (row << 1) | (0 if pixel == 0 else 1)
            if (x + 1) % 8 == 0:  # Add to bitmap every 8 bits
                bitmap.append(row)
                row = 0
        if width % 8 != 0:  # Handle leftover bits if width is not a multiple of 8
            row <<= 8 - (width % 8)
            bitmap.append(row)

    # Format the bitmap as a C array
    formatted_bitmap = ", ".join(f"0x{byte:02X}" for byte in bitmap)
    c_array = f"const unsigned char bitmap[] = {{\n{formatted_bitmap}\n}};"

    return c_array


# Example usage
image_path = "input_image.png"  # Replace with the path to your 128x64 image
bitmap_code = image_to_bitmap(image_path)

# Save the output to a file
with open("output_bitmap.h", "w") as f:
    f.write(bitmap_code)

print("Bitmap generated and saved to output_bitmap.h")
