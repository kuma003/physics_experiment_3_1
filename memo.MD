# dockerを使ってrootを立ち上げる方法

1. xming install
2. xming起動・Ip address確認
3. docker run --rm -it -e DISPLAY=192.168.10.115:0.0 -v D:/physics\_experiment\_3\_1/linux:/userhome rootproject/root
   後半のパスやipはマウントするディレクトリに応じて変更
4. 別のコンソールを開いて、 docker exec -it (コンテナのid) /bin/bash 
