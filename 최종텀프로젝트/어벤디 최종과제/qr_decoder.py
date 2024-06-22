
import requests
import cv2
import numpy as np
from pyzbar.pyzbar import decode
from pyzbar.wrapper import ZBarSymbol
from PIL import ImageFont, ImageDraw, Image

def putText(src, text, pos, font_size, font_color):
    img_pil = Image.fromarray(src)
    draw = ImageDraw.Draw(img_pil)
    font = ImageFont.truetype('fonts/gulim.ttc', font_size)
    draw.text(pos, text, font=font, fill=font_color, stroke_width = 3, stroke_fill = (0,0,0))
    return np.array(img_pil)

def get_qr_list(img, timestamp):
    qr_list = decode(img, [ZBarSymbol.QRCODE])
    for qr in qr_list:
        text = qr.data.decode('utf-8')
        x, y, width, height = qr.rect
        # 사각형의 중심 좌표를 계산합니다.
        center_x = x + (width / 2)
        #center_y = y + (height / 2)
        
        # 이미지의 중심 좌표를 가져옵니다.
        image_center_x = img.shape[1] / 2
        #image_center_x, image_center_y = frame.shape[1] / 2, frame.shape[0] / 2
        
        # x-center_x, y-center_y 값을 계산합니다.
        delta_x = int(center_x - image_center_x)
        #delta_y = center_y - image_center_y
        
        try: requests.get("http://192.48.56.2/qr?text={}&delta_x={}&timestamp={}".format(text, delta_x, timestamp))
        except: pass

    return qr_list

def qr_drawlocation(frame, qr_list):
    frame = frame.copy()
    for qr in qr_list:
        text = qr.data.decode('utf-8')
        x, y, width, height = qr.rect
        #print(text)
        polygon_Points = np.array([qr.polygon], np.int32)
        polygon_Points = polygon_Points.reshape(-1,1,2)
        cv2.polylines(frame,[polygon_Points],True, (255,0,0), 2)
        frame = putText(frame, text, (x, y), 20, (255,255,255))
        
    return frame