import os

def is_text_file(file_path):
    try:
        with open(file_path, 'rb') as f:
            chunk = f.read(1024)
        return b'\0' not in chunk
    except:
        return False


def convert_lf_to_crlf(file_path):
    with open(file_path, 'rb') as f:
        content = f.read()

    # 统一为 LF
    content = content.replace(b'\r\n', b'\n')
    content = content.replace(b'\r', b'\n')

    # 再转 CRLF
    new_content = content.replace(b'\n', b'\r\n')

    if content == new_content:
        return

    with open(file_path, 'wb') as f:
        f.write(new_content)

    print(f"[OK] {file_path}")


def process_directory_lf2crlf(root_dir, extensions=None):
    for root, dirs, files in os.walk(root_dir):
        for file in files:
            file_path = os.path.join(root, file)

            if extensions:
                if not any(file.endswith(ext) for ext in extensions):
                    continue

            if is_text_file(file_path):
                convert_lf_to_crlf(file_path)

def try_decode(raw: bytes):
    for enc in ('utf-8', 'utf-8-sig', 'gbk', 'gb18030'):
        try:
            return raw.decode(enc), enc
        except:
            pass
    return None, None

def convert_file_to_utf8(file_path: str, make_backup: bool = False) -> None:
    try:
        with open(file_path, 'rb') as f:
            raw = f.read()

        text, enc = try_decode(raw)
        if not text: print('file path ===>>', file_path)
        
        new_raw = text.encode('utf-8')  # 只转编码，不改换行

        if new_raw == raw:
            return

        if make_backup:
            bak_path = file_path + ".bak"
            if not os.path.exists(bak_path):
                with open(bak_path, 'wb') as bf:
                    bf.write(raw)

        with open(file_path, 'wb') as f:
            f.write(new_raw)

        print(f"[OK] {file_path} | {enc} -> UTF-8")

    except Exception as e:
        print(f"[FAIL] {file_path} | {e}")


def process_directory_asc2utf(root_dir: str, extensions=None):
    for root, dirs, files in os.walk(root_dir):
        for file in files:
            file_path = os.path.join(root, file)

            if extensions:
                if not any(file.endswith(ext) for ext in extensions):
                    continue

            if is_text_file(file_path):
                convert_file_to_utf8(file_path)


if __name__ == "__main__":
    project_path = r"."

    # 只处理常见代码文件（避免改资源文件）
    exts = ['.cpp', '.h', '.bat', '.c', '.pkg', '.lua', '.py']
    process_directory_lf2crlf(project_path, exts)
    process_directory_asc2utf(project_path, exts)