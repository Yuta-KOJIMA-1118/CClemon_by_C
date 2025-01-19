@echo off
rem コンパイル
gcc -pthread -o client client.c client_skill.c client_network.c client_menu.c client_utils.c client_battle.c -lws2_32

rem ビルド後に実行したい場合（オプション）
rem client.exe

rem 終了
pause
