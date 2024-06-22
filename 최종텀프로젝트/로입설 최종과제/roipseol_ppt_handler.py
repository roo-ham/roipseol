from http.server import HTTPServer, BaseHTTPRequestHandler
import fitz
import urllib.parse
import cv2
import threading

image = None
lock = threading.Lock()

for i, page in enumerate(fitz.open(f"로입설시연.pdf")):
    img = page.get_pixmap()
    img.save(f"./data/A{i}.png")


class MyHTTP(BaseHTTPRequestHandler):
    def do_GET(self):
        parsed_path = urllib.parse.urlparse(self.path)
        path = parsed_path.path
        
        # 요청된 경로 출력
        print(f"Received GET request for path: {path}")
        
        # 응답 설정
        self.send_response(200)
        self.send_header('Content-type', 'text/html')
        self.end_headers()
        
        # 응답 본문 작성
        self.wfile.write(f"Requested path: {path}".encode('utf-8'))
        try:
            if len(path) <= 1 : return
            show_pdfimg(path[1:])
        except:
            pass

def show_pdfimg(i):
    global image
    # 이미지 파일 열기
    img = cv2.imread(f"./data/{i}.png")
    if img is None:
        return
    lock.acquire()
    image = img
    lock.release()
    #img = cv2.resize(img, (1920, 1080))
    #print(img.shape)

def waitkey_loop():
    global image
    while True:
        lock.acquire()
        if image is None:
            #print("got none")
            cv2.destroyAllWindows()
            lock.release()
            continue
        cv2.namedWindow("pdf", cv2.WND_PROP_FULLSCREEN)
        cv2.setWindowProperty("pdf", cv2.WND_PROP_FULLSCREEN, cv2.WINDOW_FULLSCREEN)
        cv2.imshow("pdf", image)
        lock.release()
        key = cv2.waitKey(10)

server_address = ('', 8000)
server = HTTPServer(server_address, MyHTTP)
th = threading.Thread(target=waitkey_loop)
th.start()
server.serve_forever()
server.server_close()
th.join()
cv2.destroyAllWindows()
print("stopped")