# tesseract-parral

Run the domain

```
nohup ./a.out 401 770 /home/ketian/ChromeHeadless/snapApr01/  > begin_end.out &

nohup ./a.out 119 309 /home/ketian/ChromeHeadless/snapApr08/ > cyclops.out &

gcc task_dispatcher.c --std=c99 -o a_mobile
nohup ./a_mobile.out 0 0 /home/ketian/ChromeHeadless/snapApr01/MOBILE/  > begin_end.out &
```

Run the subdomain

```
[ketian@cyclops snapApr01]$ pwd
/home/ketian/ChromeHeadless/snapApr01

[ketian@cyclops snapApr01]$ find ./ -type f -name "*.screen.txt" | wc -l
986108


gcc ocr_dispatcher_subdomain.c --std=c99 -o a_subdomain.out
nohup ./a.subdomain.out /home/ketian/ChromeHeadless/snapAprol/subdomain/ > sub.out &

```

