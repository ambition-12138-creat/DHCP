# ============================================================
# 阶段1: 编译构建
# ============================================================
FROM ubuntu:22.04 AS builder

ENV DEBIAN_FRONTEND=noninteractive \
    TZ=Asia/Shanghai

RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential \
    cmake \
    g++ \
    qt6-base-dev \
    libgl1-mesa-dev \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /src
COPY . .

RUN cmake -S . -B build -DCMAKE_BUILD_TYPE=Release && \
    cmake --build build --parallel $(nproc)

# ============================================================
# 阶段2: 运行时镜像 (VNC 远程桌面)
# ============================================================
FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive \
    TZ=Asia/Shanghai \
    DISPLAY=:1 \
    VNC_PORT=5901 \
    NOVNC_PORT=6080 \
    VNC_RESOLUTION=1280x800 \
    VNC_DEPTH=24

RUN apt-get update && apt-get install -y --no-install-recommends \
    # Qt6 运行时
    libqt6widgets6 \
    libqt6core6 \
    libqt6gui6 \
    qt6-qpa-plugins \
    libglx0 \
    libgl1 \
    libxcb-cursor0 \
    # VNC + 轻量桌面
    tigervnc-standalone-server \
    tigervnc-common \
    novnc \
    websockify \
    openbox \
    x11-xserver-utils \
    xfonts-base \
    xterm \
    # 工具
    supervisor \
    curl \
    && rm -rf /var/lib/apt/lists/*

# 创建 VNC 密码文件目录
RUN mkdir -p /root/.vnc

# 复制编译产物
COPY --from=builder /src/build/DhcpSimulator /app/DhcpSimulator

# Supervisor 配置 — 同时管理 VNC 和 noVNC
RUN echo '[supervisord]'              >  /etc/supervisor/conf.d/app.conf && \
    echo 'nodaemon=true'              >> /etc/supervisor/conf.d/app.conf && \
    echo 'logfile=/dev/stdout'        >> /etc/supervisor/conf.d/app.conf && \
    echo 'logfile_maxbytes=0'         >> /etc/supervisor/conf.d/app.conf && \
    echo ''                           >> /etc/supervisor/conf.d/app.conf && \
    echo '[program:vnc]'              >> /etc/supervisor/conf.d/app.conf && \
    echo 'command=/usr/bin/Xtigervnc :1 -geometry 1280x800 -depth 24 -localhost no -SecurityTypes None' >> /etc/supervisor/conf.d/app.conf && \
    echo 'autorestart=true'           >> /etc/supervisor/conf.d/app.conf && \
    echo ''                           >> /etc/supervisor/conf.d/app.conf && \
    echo '[program:novnc]'            >> /etc/supervisor/conf.d/app.conf && \
    echo 'command=websockify --web /usr/share/novnc 0.0.0.0:6080 localhost:5901' >> /etc/supervisor/conf.d/app.conf && \
    echo 'autorestart=true'           >> /etc/supervisor/conf.d/app.conf && \
    echo ''                           >> /etc/supervisor/conf.d/app.conf && \
    echo '[program:openbox]'          >> /etc/supervisor/conf.d/app.conf && \
    echo 'command=openbox'            >> /etc/supervisor/conf.d/app.conf && \
    echo 'environment=DISPLAY=:1'     >> /etc/supervisor/conf.d/app.conf && \
    echo 'autorestart=true'           >> /etc/supervisor/conf.d/app.conf && \
    echo ''                           >> /etc/supervisor/conf.d/app.conf && \
    echo '[program:app]'              >> /etc/supervisor/conf.d/app.conf && \
    echo 'command=/app/DhcpSimulator' >> /etc/supervisor/conf.d/app.conf && \
    echo 'environment=DISPLAY=:1'     >> /etc/supervisor/conf.d/app.conf && \
    echo 'autorestart=true'           >> /etc/supervisor/conf.d/app.conf

EXPOSE 5901 6080

WORKDIR /app
ENTRYPOINT ["/usr/bin/supervisord", "-c", "/etc/supervisor/conf.d/app.conf"]
