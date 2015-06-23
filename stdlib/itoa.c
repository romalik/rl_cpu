char *itoa(int n, char * qbuf)
 
 {
   int r, k;
   int flag = 0;
 
   int next = 0;
   if (n < 0) {
         qbuf[next++] = '-';
         n = -n;
   }
   if (n == 0) {
         qbuf[next++] = '';
   } else {
        k = 10000;
         while (k > 0) {
                 r = n / k;
                 if (flag || r > 0) {
                         qbuf[next++] = '' + r;
                         flag = 1;
                 }
                 n -= r * k;
                 k = k / 10;
         }
   }
   qbuf[next] = 0;
   return(qbuf);
 }
