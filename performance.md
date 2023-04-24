# Тестирование времени отклика и пропускной способности

<details>
<summary>1 поток, 1 соединение</summary>

### С кешем
<pre>Running 10s test @ http://localhost:8080/
  1 threads and 1 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency     2.42ms    0.97ms  17.97ms   87.67%
    Req/Sec   414.96     39.08   460.00     85.00%
  Latency Distribution
     50%    2.03ms
     75%    3.08ms
     90%    3.45ms
     99%    4.62ms
  4135 requests in 10.01s, 1.14MB read
  Non-2xx or 3xx responses: 4135
Requests/sec:    413.02
Transfer/sec:    116.16KB</pre>

### Без кеша
<pre>Running 10s test @ http://localhost:8080/
  1 threads and 1 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency     1.83ms    0.89ms  13.89ms   91.85%
    Req/Sec   566.06    128.58   686.00     86.00%
  Latency Distribution
     50%    1.56ms
     75%    1.79ms
     90%    2.46ms
     99%    5.70ms
  5639 requests in 10.01s, 1.54MB read
  Non-2xx or 3xx responses: 5639
Requests/sec:    563.51
Transfer/sec:    157.39KB</pre>
</details>

<details>
<summary>2 потока, 2 соединения</summary>

### С кешем
<pre>Running 10s test @ http://localhost:8080/
  2 threads and 2 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency     1.00ms    1.45ms  32.78ms   94.25%
    Req/Sec     1.32k   213.05     1.66k    71.00%
  Latency Distribution
     50%  672.00us
     75%    0.86ms
     90%    1.28ms
     99%    7.67ms
  26197 requests in 10.01s, 6.97MB read
Requests/sec:   2617.56
Transfer/sec:    713.20KB</pre>

### Без кеша
<pre>Running 10s test @ http://localhost:8080/
  2 threads and 2 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency     1.72ms    1.17ms  30.69ms   96.53%
    Req/Sec   610.19     83.17   710.00     78.50%
  Latency Distribution
     50%    1.52ms
     75%    1.73ms
     90%    2.05ms
     99%    5.80ms
  12154 requests in 10.01s, 3.37MB read
  Non-2xx or 3xx responses: 12154
Requests/sec:   1214.71
Transfer/sec:    345.20KB</pre>
</details>

<details>
<summary>5 потоков, 5 соединений</summary>

### С кешем
<pre>Running 10s test @ http://localhost:8080/
  5 threads and 5 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency     3.38ms    3.06ms  48.58ms   87.84%
    Req/Sec   336.30    201.46   722.00     67.00%
  Latency Distribution
     50%    2.62ms
     75%    4.74ms
     90%    6.84ms
     99%   13.69ms
  16763 requests in 10.01s, 4.53MB read
  Non-2xx or 3xx responses: 5333
Requests/sec:   1673.88
Transfer/sec:    463.47KB</pre>

### Без кеша
<pre>Running 10s test @ http://localhost:8080/
  5 threads and 5 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency     4.04ms    2.45ms  38.44ms   85.97%
    Req/Sec   260.77     53.63   363.00     73.40%
  Latency Distribution
     50%    3.51ms
     75%    4.76ms
     90%    6.56ms
     99%   13.25ms
  13006 requests in 10.02s, 3.57MB read
  Non-2xx or 3xx responses: 10449
Requests/sec:   1297.74
Transfer/sec:    365.00KB</pre>
</details>

<details>
<summary>10 потоков, 10 соединений</summary>

### С кешем
<pre>Running 10s test @ http://localhost:8080/
  10 threads and 10 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency     3.56ms    3.61ms  52.82ms   87.72%
    Req/Sec   302.95    176.49   760.00     62.83%
  Latency Distribution
     50%    2.25ms
     75%    4.91ms
     90%    7.76ms
     99%   16.76ms
  18130 requests in 10.04s, 4.90MB read
  Socket errors: connect 4, read 0, write 0, timeout 0
  Non-2xx or 3xx responses: 4314
Requests/sec:   1806.04
Transfer/sec:    499.55KB</pre>

### Без кеша
<pre>Running 10s test @ http://localhost:8080/
  10 threads and 10 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency     5.24ms    3.70ms  45.03ms   87.60%
    Req/Sec   201.88     57.54   320.00     66.75%
  Latency Distribution
     50%    4.23ms
     75%    6.17ms
     90%    9.28ms
     99%   19.68ms
  16108 requests in 10.03s, 4.34MB read
  Socket errors: connect 2, read 0, write 0, timeout 0
  Non-2xx or 3xx responses: 8095
Requests/sec:   1605.71
Transfer/sec:    443.03KB</pre>
</details>