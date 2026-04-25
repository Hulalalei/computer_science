#!/usr/bin/env python3
# -*- coding: utf-8 -*-

"""
静态文件服务器 - 将 ./res 目录中的文件提供给浏览器访问
支持：
- 自动识别 MIME 类型（HTML、CSS、JS、图片等）
- 目录列表（类似 Nginx autoindex）
- 默认索引文件（index.html）
- 路径遍历攻击防护
"""

import os
import sys
from flask import Flask, send_from_directory, abort, redirect, url_for, request, escape
from urllib.parse import unquote

# 配置
RES_DIR = os.path.join(os.getcwd(), 'res')      # 资源目录
PORT = 5000                                     # 监听端口
HOST = '0.0.0.0'                                # 监听所有网络接口

# 创建 Flask 应用
app = Flask(__name__)


def is_safe_path(base_dir, target_path):
    """
    检查目标路径是否在基础目录内，防止路径遍历攻击。
    """
    # 规范化路径，消除 '..' 和符号链接等
    base_dir = os.path.realpath(base_dir)
    target_path = os.path.realpath(target_path)
    return os.path.commonpath([base_dir, target_path]) == base_dir


def list_directory(rel_path):
    """
    生成指定目录的 HTML 文件列表。
    rel_path: 相对于 RES_DIR 的路径（可为空字符串或 None，表示根目录）
    返回 HTML 字符串。
    """
    if rel_path is None:
        rel_path = ''
    # 获取绝对路径
    abs_path = os.path.realpath(os.path.join(RES_DIR, rel_path))
    if not is_safe_path(RES_DIR, abs_path) or not os.path.isdir(abs_path):
        abort(404)

    # 收集文件/目录信息
    items = []
    try:
        for name in os.listdir(abs_path):
            # 跳过隐藏文件（可选）
            # if name.startswith('.'):
            #     continue
            full = os.path.join(abs_path, name)
            is_dir = os.path.isdir(full)
            items.append((name, is_dir))
    except OSError:
        abort(404)

    # 按名称排序，目录在前（可选）
    items.sort(key=lambda x: (not x[1], x[0].lower()))

    # 生成 HTML
    title = f'Index of /{rel_path}' if rel_path else 'Index of /'
    rows = []
    # 父目录链接（不在根目录时显示）
    if rel_path and rel_path != '':
        parent_path = os.path.dirname(rel_path.rstrip('/'))
        if parent_path == '':
            parent_href = '../'
        else:
            parent_href = f'../{parent_path}/'
        rows.append(f'<tr><td><a href="{escape(parent_href)}">../</a></td><td> - </td></tr>')

    for name, is_dir in items:
        # 显示名，目录加斜杠
        display_name = name + '/' if is_dir else name
        # 链接 URL
        if is_dir:
            href = f'./{name}/'   # 目录以斜杠结尾，便于浏览器正确处理相对路径
        else:
            href = f'./{name}'
        # 获取文件大小（仅文件）
        size = ''
        if not is_dir:
            try:
                size_bytes = os.path.getsize(os.path.join(abs_path, name))
                if size_bytes < 1024:
                    size = f'{size_bytes} B'
                elif size_bytes < 1024 * 1024:
                    size = f'{size_bytes / 1024:.1f} KB'
                else:
                    size = f'{size_bytes / (1024 * 1024):.1f} MB'
            except OSError:
                size = '?'
        rows.append(f'<tr><td><a href="{escape(href)}">{escape(display_name)}</a></td><td>{escape(size)}</td></tr>')

    html = f'''<!DOCTYPE html>
<html>
<head>
    <meta charset="utf-8">
    <title>{escape(title)}</title>
    <style>
        body {{ font-family: monospace; margin: 40px; }}
        h1 {{ font-size: 1.5rem; }}
        table {{ border-collapse: collapse; width: 100%; }}
        th, td {{ text-align: left; padding: 5px 10px; }}
        th {{ background-color: #f2f2f2; }}
        tr:nth-child(even) {{ background-color: #f9f9f9; }}
        a {{ text-decoration: none; color: #0366d6; }}
        a:hover {{ text-decoration: underline; }}
    </style>
</head>
<body>
    <h1>{escape(title)}</h1>
    <table>
        <tr><th>Name</th><th>Size</th></tr>
        {''.join(rows)}
    </table>
</body>
</html>'''
    return html


@app.route('/', defaults={'filename': ''})
@app.route('/<path:filename>')
def serve_static(filename):
    """
    核心路由：处理所有对文件和目录的请求。
    """
    # 1. 安全处理：解码 URL 编码的路径，并标准化
    filename = unquote(filename)  # 处理 %20 等
    # 防止空字符注入
    if '\0' in filename:
        abort(404)

    # 构建目标路径的绝对路径
    target = os.path.join(RES_DIR, filename)
    target = os.path.realpath(target)

    # 检查路径是否安全（必须在 RES_DIR 内）
    if not is_safe_path(RES_DIR, target):
        abort(404)

    # 2. 如果目标不存在，返回 404
    if not os.path.exists(target):
        abort(404)

    # 3. 如果是文件，直接发送（Flask 会自动处理 MIME 类型）
    if os.path.isfile(target):
        # 使用 send_from_directory 发送（额外安全层）
        # 注意：需要提取相对于 RES_DIR 的路径
        rel_path = os.path.relpath(target, RES_DIR)
        # 确保相对路径不会越界（relpath 可能为 '..' 开头的，但已由 is_safe_path 保证）
        return send_from_directory(RES_DIR, rel_path)

    # 4. 如果是目录，处理目录请求
    if os.path.isdir(target):
        # 如果请求路径不以 '/' 结尾，重定向添加斜杠（避免相对路径问题）
        if not filename.endswith('/') and filename != '':
            # 重定向到加斜杠的 URL
            return redirect(url_for('serve_static', filename=filename + '/'))
        
        # 尝试寻找目录下的 index.html
        index_path = os.path.join(target, 'index.html')
        if os.path.isfile(index_path):
            # 存在默认索引文件，发送它
            return send_from_directory(RES_DIR, os.path.join(filename, 'index.html'))
        else:
            # 无索引文件，生成目录列表
            html = list_directory(filename.rstrip('/'))  # 去掉末尾斜杠
            return html

    # 其他情况（如特殊文件）返回 404
    abort(404)


# 启动前的准备：确保 res 目录存在
def check_res_dir():
    if not os.path.exists(RES_DIR):
        try:
            os.makedirs(RES_DIR)
            print(f"[信息] 已创建资源目录: {RES_DIR}")
            print("[提示] 请将您的 HTML/CSS/JS 等文件放入此目录，然后刷新浏览器。")
        except OSError as e:
            print(f"[错误] 无法创建目录 {RES_DIR}: {e}", file=sys.stderr)
            sys.exit(1)
    else:
        print(f"[信息] 资源目录已存在: {RES_DIR}")


if __name__ == '__main__':
    check_res_dir()
    print(f"服务器启动，监听地址: http://{HOST}:{PORT}")
    print(f"服务目录: {RES_DIR}")
    print("按 Ctrl+C 停止服务器")
    app.run(host=HOST, port=PORT, debug=False)   # 生产环境建议 debug=False
