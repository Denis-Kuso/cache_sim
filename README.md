## What is it 

Simulates behaviour of an L1 cache. Given a trace file of memory
references(output by Valgrind) and a parametrization of cache
organisation/architecture, outputs the number of hits, misses and evictions.
Replacement policy for evictions is least recently used (LRU).

### Asumptions

- 64 bit address
- every memory access propery aligned (no crossings of block bounds)

### Usage

```bash
./cache_sim -s 2 -E 2 -b 4 -t traces/tracefile.trace
```

> Example output to stdout:
`>hits: 12, misses: 2, evictions: 1`

> And to file `.cache_output`:
`12 2 1`

> Options:

 - `h`: display usage
 - `v`: display result (hit/miss, eviction) for every reference
 - `s <num>`: number of bits to determine set index
 - `E <num>`: number of lines per set
 - `b <num>`: number of bits available for data offset
 - `t <filename>`: file containing memory traces (valgrind)

### Constraints:

 - $s \geq 1$
 - $b \geq 1$
 - $E \geq 1$
 - $64 - (s + b) > 0$, where 64 is the address size
