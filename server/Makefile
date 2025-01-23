# コンパイラの設定
CC = gcc

# コンパイラのオプション
# Wall: 警告を表示
# Wextra: 追加の警告を表示
# g: デバッグ情報を含める(gdbでデバッグ可能)
# pthread: pthreadライブラリをリンク
CFLAGS = -Wall -Wextra -g -pthread

# 出力ファイル名
TARGET = server

# ソースファイルのリスト
SRCS = server.c skills.c room.c network.c room_actions.c battle.c

# オブジェクトファイルのリスト
OBJS = $(SRCS:.c=.o)

# デフォルトのターゲット
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

# 各ソースファイルからオブジェクトファイルを作成
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# クリーンアップ
clean:
	rm -f $(OBJS) $(TARGET)

# .PHONYターゲット（クリーンはファイル名でないため）
.PHONY: clean
