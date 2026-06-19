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
    x11-utils \
    xfonts-base \
    xterm \
    # 工具
    supervisor \
    curl \
    && rm -rf /var/lib/apt/lists/*

RUN mkdir -p /root/.vnc

# 复制编译产物
COPY --from=builder /src/build/DhcpSimulator /app/DhcpSimulator

# 启动脚本 — 按顺序等待依赖就绪
RUN echo '#!/bin/bash'                                  >  /app/startup.sh && \
    echo 'set -e'                                       >> /app/startup.sh && \
    echo ''                                             >> /app/startup.sh && \
    echo '# 等待 VNC 服务就绪'                          >> /app/startup.sh && \
    echo 'for i in $(seq 1 30); do'                     >> /app/startup.sh && \
    echo '  if xdpyinfo -display :1 >/dev/null 2>&1; then break; fi' >> /app/startup.sh && \
    echo '  sleep 1'                                    >> /app/startup.sh && \
    echo 'done'                                         >> /app/startup.sh && \
    echo ''                                             >> /app/startup.sh && \
    echo '# 启动窗口管理器（后台）'                     >> /app/startup.sh && \
    echo 'openbox &'                                    >> /app/startup.sh && \
    echo 'sleep 2'                                      >> /app/startup.sh && \
    echo ''                                             >> /app/startup.sh && \
    echo '# 启动 DHCP 模拟器'                           >> /app/startup.sh && \
    echo 'exec /app/DhcpSimulator'                      >> /app/startup.sh && \
    chmod +x /app/startup.sh

# Supervisor 配置
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
    echo '[program:app]'              >> /etc/supervisor/conf.d/app.conf && \
    echo 'command=/app/startup.sh'    >> /etc/supervisor/conf.d/app.conf && \
    echo 'environment=DISPLAY=:1'     >> /etc/supervisor/conf.d/app.conf && \
    echo 'autorestart=true'           >> /etc/supervisor/conf.d/app.conf && \
    echo 'startsecs=5'                >> /etc/supervisor/conf.d/app.conf

EXPOSE 5901 6080

WORKDIR /app
ENTRYPOINT ["/usr/bin/supervisord", "-c", "/etc/supervisor/conf.d/app.conf"]
