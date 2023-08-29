// Lifted basically 1:1 from jomme. Should be pretty close to what the original Jedi Knight source code releases contained, save for a few smaller changes
//
#include "demoCut.h"
#include "jkaStuff.h"
#include "otherGameStuff.h"
#include "otherGameStuff2.h"
#include "stateFields.h"

#ifdef _DONETPROFILE_
#include "INetProfile.h"
#endif

//#define _NEWHUFFTABLE_		// Build "c:\\netchan.bin"
//#define _USINGNEWHUFFTABLE_		// Build a new frequency table to cut and paste.


#define FASTHUFFMAN // Based on: https://github.com/mightycow/uberdemotools/commit/685b132abc4803f4c813fa07928cd9a4099e5d59


// Fast Huffman decoder from:
// from: https://github.com/mightycow/uberdemotools/commit/685b132abc4803f4c813fa07928cd9a4099e5d59
#ifdef FASTHUFFMAN
static const uint16_t HuffmanDecoderTable[2048] =
{
	2512, 2182, 512, 2763, 1859, 2808, 512, 2360, 1918, 1988, 512, 1803, 2158, 2358, 512, 2180,
	1798, 2053, 512, 1804, 2603, 1288, 512, 2166, 2285, 2167, 512, 1281, 1640, 2767, 512, 1664,
	1731, 2116, 512, 2788, 1791, 1808, 512, 1840, 2153, 1921, 512, 2708, 2723, 1549, 512, 2046,
	1893, 2717, 512, 2602, 1801, 1288, 512, 1568, 2480, 2062, 512, 1281, 2145, 2711, 512, 1543,
	1909, 2150, 512, 2077, 2338, 2762, 512, 2162, 1794, 2024, 512, 2168, 1922, 2447, 512, 2334,
	1857, 2117, 512, 2100, 2240, 1288, 512, 2186, 2321, 1908, 512, 1281, 1640, 2242, 512, 1664,
	1731, 2729, 512, 2633, 1791, 1919, 512, 2184, 1917, 1802, 512, 2710, 1795, 1549, 512, 2172,
	2375, 2789, 512, 2171, 2187, 1288, 512, 1568, 2095, 2163, 512, 1281, 1858, 1923, 512, 1543,
	2374, 2446, 512, 2181, 1859, 2160, 512, 2183, 1918, 1988, 512, 1803, 2161, 2751, 512, 2413,
	1798, 2529, 512, 1804, 2344, 1288, 512, 2404, 2156, 2786, 512, 1281, 1640, 2641, 512, 1664,
	1731, 2052, 512, 2170, 1791, 1808, 512, 1840, 2395, 1921, 512, 2586, 2319, 1549, 512, 2046,
	1893, 2101, 512, 2159, 1801, 1288, 512, 1568, 2247, 2773, 512, 1281, 2365, 2410, 512, 1543,
	1909, 2781, 512, 2097, 2411, 2740, 512, 2396, 1794, 2024, 512, 2734, 1922, 2733, 512, 2112,
	1857, 2528, 512, 2593, 2079, 1288, 512, 2648, 2143, 1908, 512, 1281, 1640, 2770, 512, 1664,
	1731, 2169, 512, 2714, 1791, 1919, 512, 2185, 1917, 1802, 512, 2398, 1795, 1549, 512, 2098,
	2801, 2361, 512, 2400, 2328, 1288, 512, 1568, 2783, 2713, 512, 1281, 1858, 1923, 512, 1543,
	2816, 2182, 512, 2497, 1859, 2397, 512, 2794, 1918, 1988, 512, 1803, 2158, 2772, 512, 2180,
	1798, 2053, 512, 1804, 2464, 1288, 512, 2166, 2285, 2167, 512, 1281, 1640, 2764, 512, 1664,
	1731, 2116, 512, 2620, 1791, 1808, 512, 1840, 2153, 1921, 512, 2716, 2384, 1549, 512, 2046,
	1893, 2448, 512, 2722, 1801, 1288, 512, 1568, 2472, 2062, 512, 1281, 2145, 2376, 512, 1543,
	1909, 2150, 512, 2077, 2366, 2709, 512, 2162, 1794, 2024, 512, 2168, 1922, 2735, 512, 2407,
	1857, 2117, 512, 2100, 2240, 1288, 512, 2186, 2779, 1908, 512, 1281, 1640, 2242, 512, 1664,
	1731, 2359, 512, 2705, 1791, 1919, 512, 2184, 1917, 1802, 512, 2642, 1795, 1549, 512, 2172,
	2394, 2645, 512, 2171, 2187, 1288, 512, 1568, 2095, 2163, 512, 1281, 1858, 1923, 512, 1543,
	2450, 2771, 512, 2181, 1859, 2160, 512, 2183, 1918, 1988, 512, 1803, 2161, 2585, 512, 2403,
	1798, 2619, 512, 1804, 2777, 1288, 512, 2355, 2156, 2362, 512, 1281, 1640, 2380, 512, 1664,
	1731, 2052, 512, 2170, 1791, 1808, 512, 1840, 2811, 1921, 512, 2402, 2601, 1549, 512, 2046,
	1893, 2101, 512, 2159, 1801, 1288, 512, 1568, 2247, 2719, 512, 1281, 2747, 2776, 512, 1543,
	1909, 2725, 512, 2097, 2445, 2765, 512, 2638, 1794, 2024, 512, 2444, 1922, 2774, 512, 2112,
	1857, 2727, 512, 2644, 2079, 1288, 512, 2800, 2143, 1908, 512, 1281, 1640, 2580, 512, 1664,
	1731, 2169, 512, 2646, 1791, 1919, 512, 2185, 1917, 1802, 512, 2588, 1795, 1549, 512, 2098,
	2322, 2504, 512, 2623, 2350, 1288, 512, 1568, 2323, 2721, 512, 1281, 1858, 1923, 512, 1543,
	2512, 2182, 512, 2746, 1859, 2798, 512, 2360, 1918, 1988, 512, 1803, 2158, 2358, 512, 2180,
	1798, 2053, 512, 1804, 2745, 1288, 512, 2166, 2285, 2167, 512, 1281, 1640, 2806, 512, 1664,
	1731, 2116, 512, 2796, 1791, 1808, 512, 1840, 2153, 1921, 512, 2582, 2761, 1549, 512, 2046,
	1893, 2793, 512, 2647, 1801, 1288, 512, 1568, 2480, 2062, 512, 1281, 2145, 2738, 512, 1543,
	1909, 2150, 512, 2077, 2338, 2715, 512, 2162, 1794, 2024, 512, 2168, 1922, 2447, 512, 2334,
	1857, 2117, 512, 2100, 2240, 1288, 512, 2186, 2321, 1908, 512, 1281, 1640, 2242, 512, 1664,
	1731, 2795, 512, 2750, 1791, 1919, 512, 2184, 1917, 1802, 512, 2732, 1795, 1549, 512, 2172,
	2375, 2604, 512, 2171, 2187, 1288, 512, 1568, 2095, 2163, 512, 1281, 1858, 1923, 512, 1543,
	2374, 2446, 512, 2181, 1859, 2160, 512, 2183, 1918, 1988, 512, 1803, 2161, 2813, 512, 2413,
	1798, 2529, 512, 1804, 2344, 1288, 512, 2404, 2156, 2743, 512, 1281, 1640, 2748, 512, 1664,
	1731, 2052, 512, 2170, 1791, 1808, 512, 1840, 2395, 1921, 512, 2637, 2319, 1549, 512, 2046,
	1893, 2101, 512, 2159, 1801, 1288, 512, 1568, 2247, 2812, 512, 1281, 2365, 2410, 512, 1543,
	1909, 2799, 512, 2097, 2411, 2802, 512, 2396, 1794, 2024, 512, 2649, 1922, 2595, 512, 2112,
	1857, 2528, 512, 2790, 2079, 1288, 512, 2634, 2143, 1908, 512, 1281, 1640, 2724, 512, 1664,
	1731, 2169, 512, 2730, 1791, 1919, 512, 2185, 1917, 1802, 512, 2398, 1795, 1549, 512, 2098,
	2605, 2361, 512, 2400, 2328, 1288, 512, 1568, 2787, 2810, 512, 1281, 1858, 1923, 512, 1543,
	2803, 2182, 512, 2497, 1859, 2397, 512, 2758, 1918, 1988, 512, 1803, 2158, 2598, 512, 2180,
	1798, 2053, 512, 1804, 2464, 1288, 512, 2166, 2285, 2167, 512, 1281, 1640, 2726, 512, 1664,
	1731, 2116, 512, 2583, 1791, 1808, 512, 1840, 2153, 1921, 512, 2712, 2384, 1549, 512, 2046,
	1893, 2448, 512, 2639, 1801, 1288, 512, 1568, 2472, 2062, 512, 1281, 2145, 2376, 512, 1543,
	1909, 2150, 512, 2077, 2366, 2731, 512, 2162, 1794, 2024, 512, 2168, 1922, 2766, 512, 2407,
	1857, 2117, 512, 2100, 2240, 1288, 512, 2186, 2809, 1908, 512, 1281, 1640, 2242, 512, 1664,
	1731, 2359, 512, 2587, 1791, 1919, 512, 2184, 1917, 1802, 512, 2643, 1795, 1549, 512, 2172,
	2394, 2635, 512, 2171, 2187, 1288, 512, 1568, 2095, 2163, 512, 1281, 1858, 1923, 512, 1543,
	2450, 2749, 512, 2181, 1859, 2160, 512, 2183, 1918, 1988, 512, 1803, 2161, 2778, 512, 2403,
	1798, 2791, 512, 1804, 2775, 1288, 512, 2355, 2156, 2362, 512, 1281, 1640, 2380, 512, 1664,
	1731, 2052, 512, 2170, 1791, 1808, 512, 1840, 2805, 1921, 512, 2402, 2741, 1549, 512, 2046,
	1893, 2101, 512, 2159, 1801, 1288, 512, 1568, 2247, 2769, 512, 1281, 2739, 2780, 512, 1543,
	1909, 2737, 512, 2097, 2445, 2596, 512, 2757, 1794, 2024, 512, 2444, 1922, 2599, 512, 2112,
	1857, 2804, 512, 2744, 2079, 1288, 512, 2707, 2143, 1908, 512, 1281, 1640, 2782, 512, 1664,
	1731, 2169, 512, 2742, 1791, 1919, 512, 2185, 1917, 1802, 512, 2718, 1795, 1549, 512, 2098,
	2322, 2504, 512, 2581, 2350, 1288, 512, 1568, 2323, 2597, 512, 1281, 1858, 1923, 512, 1543,
	2512, 2182, 512, 2763, 1859, 2808, 512, 2360, 1918, 1988, 512, 1803, 2158, 2358, 512, 2180,
	1798, 2053, 512, 1804, 2603, 1288, 512, 2166, 2285, 2167, 512, 1281, 1640, 2767, 512, 1664,
	1731, 2116, 512, 2788, 1791, 1808, 512, 1840, 2153, 1921, 512, 2708, 2723, 1549, 512, 2046,
	1893, 2717, 512, 2602, 1801, 1288, 512, 1568, 2480, 2062, 512, 1281, 2145, 2711, 512, 1543,
	1909, 2150, 512, 2077, 2338, 2762, 512, 2162, 1794, 2024, 512, 2168, 1922, 2447, 512, 2334,
	1857, 2117, 512, 2100, 2240, 1288, 512, 2186, 2321, 1908, 512, 1281, 1640, 2242, 512, 1664,
	1731, 2729, 512, 2633, 1791, 1919, 512, 2184, 1917, 1802, 512, 2710, 1795, 1549, 512, 2172,
	2375, 2789, 512, 2171, 2187, 1288, 512, 1568, 2095, 2163, 512, 1281, 1858, 1923, 512, 1543,
	2374, 2446, 512, 2181, 1859, 2160, 512, 2183, 1918, 1988, 512, 1803, 2161, 2751, 512, 2413,
	1798, 2529, 512, 1804, 2344, 1288, 512, 2404, 2156, 2786, 512, 1281, 1640, 2641, 512, 1664,
	1731, 2052, 512, 2170, 1791, 1808, 512, 1840, 2395, 1921, 512, 2586, 2319, 1549, 512, 2046,
	1893, 2101, 512, 2159, 1801, 1288, 512, 1568, 2247, 2773, 512, 1281, 2365, 2410, 512, 1543,
	1909, 2781, 512, 2097, 2411, 2740, 512, 2396, 1794, 2024, 512, 2734, 1922, 2733, 512, 2112,
	1857, 2528, 512, 2593, 2079, 1288, 512, 2648, 2143, 1908, 512, 1281, 1640, 2770, 512, 1664,
	1731, 2169, 512, 2714, 1791, 1919, 512, 2185, 1917, 1802, 512, 2398, 1795, 1549, 512, 2098,
	2801, 2361, 512, 2400, 2328, 1288, 512, 1568, 2783, 2713, 512, 1281, 1858, 1923, 512, 1543,
	3063, 2182, 512, 2497, 1859, 2397, 512, 2794, 1918, 1988, 512, 1803, 2158, 2772, 512, 2180,
	1798, 2053, 512, 1804, 2464, 1288, 512, 2166, 2285, 2167, 512, 1281, 1640, 2764, 512, 1664,
	1731, 2116, 512, 2620, 1791, 1808, 512, 1840, 2153, 1921, 512, 2716, 2384, 1549, 512, 2046,
	1893, 2448, 512, 2722, 1801, 1288, 512, 1568, 2472, 2062, 512, 1281, 2145, 2376, 512, 1543,
	1909, 2150, 512, 2077, 2366, 2709, 512, 2162, 1794, 2024, 512, 2168, 1922, 2735, 512, 2407,
	1857, 2117, 512, 2100, 2240, 1288, 512, 2186, 2779, 1908, 512, 1281, 1640, 2242, 512, 1664,
	1731, 2359, 512, 2705, 1791, 1919, 512, 2184, 1917, 1802, 512, 2642, 1795, 1549, 512, 2172,
	2394, 2645, 512, 2171, 2187, 1288, 512, 1568, 2095, 2163, 512, 1281, 1858, 1923, 512, 1543,
	2450, 2771, 512, 2181, 1859, 2160, 512, 2183, 1918, 1988, 512, 1803, 2161, 2585, 512, 2403,
	1798, 2619, 512, 1804, 2777, 1288, 512, 2355, 2156, 2362, 512, 1281, 1640, 2380, 512, 1664,
	1731, 2052, 512, 2170, 1791, 1808, 512, 1840, 2811, 1921, 512, 2402, 2601, 1549, 512, 2046,
	1893, 2101, 512, 2159, 1801, 1288, 512, 1568, 2247, 2719, 512, 1281, 2747, 2776, 512, 1543,
	1909, 2725, 512, 2097, 2445, 2765, 512, 2638, 1794, 2024, 512, 2444, 1922, 2774, 512, 2112,
	1857, 2727, 512, 2644, 2079, 1288, 512, 2800, 2143, 1908, 512, 1281, 1640, 2580, 512, 1664,
	1731, 2169, 512, 2646, 1791, 1919, 512, 2185, 1917, 1802, 512, 2588, 1795, 1549, 512, 2098,
	2322, 2504, 512, 2623, 2350, 1288, 512, 1568, 2323, 2721, 512, 1281, 1858, 1923, 512, 1543,
	2512, 2182, 512, 2746, 1859, 2798, 512, 2360, 1918, 1988, 512, 1803, 2158, 2358, 512, 2180,
	1798, 2053, 512, 1804, 2745, 1288, 512, 2166, 2285, 2167, 512, 1281, 1640, 2806, 512, 1664,
	1731, 2116, 512, 2796, 1791, 1808, 512, 1840, 2153, 1921, 512, 2582, 2761, 1549, 512, 2046,
	1893, 2793, 512, 2647, 1801, 1288, 512, 1568, 2480, 2062, 512, 1281, 2145, 2738, 512, 1543,
	1909, 2150, 512, 2077, 2338, 2715, 512, 2162, 1794, 2024, 512, 2168, 1922, 2447, 512, 2334,
	1857, 2117, 512, 2100, 2240, 1288, 512, 2186, 2321, 1908, 512, 1281, 1640, 2242, 512, 1664,
	1731, 2795, 512, 2750, 1791, 1919, 512, 2184, 1917, 1802, 512, 2732, 1795, 1549, 512, 2172,
	2375, 2604, 512, 2171, 2187, 1288, 512, 1568, 2095, 2163, 512, 1281, 1858, 1923, 512, 1543,
	2374, 2446, 512, 2181, 1859, 2160, 512, 2183, 1918, 1988, 512, 1803, 2161, 2813, 512, 2413,
	1798, 2529, 512, 1804, 2344, 1288, 512, 2404, 2156, 2743, 512, 1281, 1640, 2748, 512, 1664,
	1731, 2052, 512, 2170, 1791, 1808, 512, 1840, 2395, 1921, 512, 2637, 2319, 1549, 512, 2046,
	1893, 2101, 512, 2159, 1801, 1288, 512, 1568, 2247, 2812, 512, 1281, 2365, 2410, 512, 1543,
	1909, 2799, 512, 2097, 2411, 2802, 512, 2396, 1794, 2024, 512, 2649, 1922, 2595, 512, 2112,
	1857, 2528, 512, 2790, 2079, 1288, 512, 2634, 2143, 1908, 512, 1281, 1640, 2724, 512, 1664,
	1731, 2169, 512, 2730, 1791, 1919, 512, 2185, 1917, 1802, 512, 2398, 1795, 1549, 512, 2098,
	2605, 2361, 512, 2400, 2328, 1288, 512, 1568, 2787, 2810, 512, 1281, 1858, 1923, 512, 1543,
	2803, 2182, 512, 2497, 1859, 2397, 512, 2758, 1918, 1988, 512, 1803, 2158, 2598, 512, 2180,
	1798, 2053, 512, 1804, 2464, 1288, 512, 2166, 2285, 2167, 512, 1281, 1640, 2726, 512, 1664,
	1731, 2116, 512, 2583, 1791, 1808, 512, 1840, 2153, 1921, 512, 2712, 2384, 1549, 512, 2046,
	1893, 2448, 512, 2639, 1801, 1288, 512, 1568, 2472, 2062, 512, 1281, 2145, 2376, 512, 1543,
	1909, 2150, 512, 2077, 2366, 2731, 512, 2162, 1794, 2024, 512, 2168, 1922, 2766, 512, 2407,
	1857, 2117, 512, 2100, 2240, 1288, 512, 2186, 2809, 1908, 512, 1281, 1640, 2242, 512, 1664,
	1731, 2359, 512, 2587, 1791, 1919, 512, 2184, 1917, 1802, 512, 2643, 1795, 1549, 512, 2172,
	2394, 2635, 512, 2171, 2187, 1288, 512, 1568, 2095, 2163, 512, 1281, 1858, 1923, 512, 1543,
	2450, 2749, 512, 2181, 1859, 2160, 512, 2183, 1918, 1988, 512, 1803, 2161, 2778, 512, 2403,
	1798, 2791, 512, 1804, 2775, 1288, 512, 2355, 2156, 2362, 512, 1281, 1640, 2380, 512, 1664,
	1731, 2052, 512, 2170, 1791, 1808, 512, 1840, 2805, 1921, 512, 2402, 2741, 1549, 512, 2046,
	1893, 2101, 512, 2159, 1801, 1288, 512, 1568, 2247, 2769, 512, 1281, 2739, 2780, 512, 1543,
	1909, 2737, 512, 2097, 2445, 2596, 512, 2757, 1794, 2024, 512, 2444, 1922, 2599, 512, 2112,
	1857, 2804, 512, 2744, 2079, 1288, 512, 2707, 2143, 1908, 512, 1281, 1640, 2782, 512, 1664,
	1731, 2169, 512, 2742, 1791, 1919, 512, 2185, 1917, 1802, 512, 2718, 1795, 1549, 512, 2098,
	2322, 2504, 512, 2581, 2350, 1288, 512, 1568, 2323, 2597, 512, 1281, 1858, 1923, 512, 1543
};

static const uint16_t HuffmanEncoderTable[256] =
{
	34, 437, 1159, 1735, 2584, 280, 263, 1014, 341, 839, 1687, 183, 311, 726, 920, 2761,
	599, 1417, 7945, 8073, 7642, 16186, 8890, 12858, 3913, 6362, 2746, 13882, 7866, 1080, 1273, 3400,
	886, 3386, 1097, 11482, 15450, 16282, 12506, 15578, 2377, 6858, 826, 330, 10010, 12042, 8009, 1928,
	631, 3128, 3832, 6521, 1336, 2840, 217, 5657, 121, 3865, 6553, 6426, 4666, 3017, 5193, 7994,
	3320, 1287, 1991, 71, 536, 1304, 2057, 1801, 5081, 1594, 11642, 14106, 6617, 10938, 7290, 13114,
	4809, 2522, 5818, 14010, 7482, 5914, 7738, 9018, 3450, 11450, 5897, 2697, 3193, 4185, 3769, 3464,
	3897, 968, 6841, 6393, 2425, 775, 1048, 5369, 454, 648, 3033, 3145, 2440, 2297, 200, 2872,
	2136, 2248, 1144, 1944, 1431, 1031, 376, 408, 1208, 3608, 2616, 1848, 1784, 1671, 135, 1623,
	502, 663, 1223, 2007, 248, 2104, 24, 2168, 1656, 3704, 1400, 1864, 7353, 7241, 2073, 1241,
	4889, 5690, 6153, 15738, 698, 5210, 1722, 986, 12986, 3994, 3642, 9306, 4794, 794, 16058, 7066,
	4425, 8090, 4922, 714, 11738, 7194, 12762, 7450, 5001, 1562, 11834, 13402, 9914, 3290, 3258, 5338,
	905, 15386, 9178, 15306, 3162, 15050, 15930, 10650, 15674, 8522, 8250, 7114, 10714, 14362, 9786, 2266,
	1352, 4153, 1496, 518, 151, 15482, 12410, 2952, 7961, 8906, 1114, 58, 4570, 7258, 13530, 474,
	9, 15258, 3546, 6170, 4314, 2970, 7386, 14666, 7130, 6474, 14554, 5514, 15322, 3098, 15834, 3978,
	3353, 2329, 2458, 12170, 570, 1818, 11578, 14618, 1175, 8986, 4218, 9754, 8762, 392, 8282, 11290,
	7546, 3850, 11354, 12298, 15642, 14986, 8666, 20491, 90, 13706, 12186, 6794, 11162, 10458, 759, 582
};


// MOHAA Scrambled String table
// Scrambled string conversion (write)
const uint8_t StrCharToNetByte[256] =
{
	254, 120, 89, 13, 27, 73, 103, 78, 74, 102, 21, 117, 76, 86, 238, 96, 88, 62, 59, 60,
	40, 84, 52, 119, 251, 51, 75, 121, 192, 85, 44, 54, 114, 87, 25, 53, 35, 224, 67, 31,
	82, 41, 45, 99, 233, 112, 255, 11, 46, 115, 8, 32, 19, 100, 110, 95, 116, 48, 58, 107,
	70, 91, 104, 81, 118, 109, 36, 24, 17, 39, 43, 65, 49, 83, 56, 57, 33, 64, 80, 28,
	184, 160, 18, 105, 42, 20, 194, 38, 29, 26, 61, 50, 9, 90, 37, 128, 79, 2, 108, 34,
	4, 0, 47, 12, 101, 10, 92, 15, 5, 7, 22, 55, 23, 14, 3, 1, 66, 16, 63, 30,
	6, 97, 111, 248, 72, 197, 191, 122, 176, 245, 250, 68, 195, 77, 232, 106, 228, 93, 240, 98,
	208, 69, 164, 144, 186, 222, 94, 246, 148, 170, 244, 190, 205, 234, 252, 202, 230, 239, 174, 225,
	226, 209, 236, 216, 237, 151, 149, 231, 129, 188, 200, 172, 204, 154, 168, 71, 133, 217, 196, 223,
	134, 253, 173, 177, 219, 235, 214, 182, 132, 227, 183, 175, 137, 152, 158, 221, 243, 150, 210, 136,
	167, 211, 179, 193, 218, 124, 140, 178, 213, 249, 185, 113, 127, 220, 180, 145, 138, 198, 123, 162,
	189, 203, 166, 126, 159, 156, 212, 207, 146, 181, 247, 139, 142, 169, 242, 241, 171, 187, 153, 135,
	201, 155, 161, 125, 163, 130, 229, 206, 165, 157, 141, 147, 143, 199, 215, 131
};

// Scrambled string conversion (read)
const uint8_t NetByteToStrChar[256] =
{
	101, 115, 97, 114, 100, 108, 120, 109, 50, 92, 105, 47, 103, 3, 113, 107, 117, 68, 82, 52,
	85, 10, 110, 112, 67, 34, 89, 4, 79, 88, 119, 39, 51, 76, 99, 36, 66, 94, 87, 69,
	20, 41, 84, 70, 30, 42, 48, 102, 57, 72, 91, 25, 22, 35, 31, 111, 74, 75, 58, 18,
	19, 90, 17, 118, 77, 71, 116, 38, 131, 141, 60, 175, 124, 5, 8, 26, 12, 133, 7, 96,
	78, 63, 40, 73, 21, 29, 13, 33, 16, 2, 93, 61, 106, 137, 146, 55, 15, 121, 139, 43,
	53, 104, 9, 6, 62, 83, 135, 59, 98, 65, 54, 122, 45, 211, 32, 49, 56, 11, 64, 23,
	1, 27, 127, 218, 205, 243, 223, 212, 95, 168, 245, 255, 188, 176, 180, 239, 199, 192, 216, 231,
	206, 250, 232, 252, 143, 215, 228, 251, 148, 166, 197, 165, 193, 238, 173, 241, 225, 249, 194, 224,
	81, 242, 219, 244, 142, 248, 222, 200, 174, 233, 149, 236, 171, 182, 158, 191, 128, 183, 207, 202,
	214, 229, 187, 190, 80, 210, 144, 237, 169, 220, 151, 126, 28, 203, 86, 132, 178, 125, 217, 253,
	170, 240, 155, 221, 172, 152, 247, 227, 140, 161, 198, 201, 226, 208, 186, 254, 163, 177, 204, 184,
	213, 195, 145, 179, 37, 159, 160, 189, 136, 246, 156, 167, 134, 44, 153, 185, 162, 164, 14, 157,
	138, 235, 234, 196, 150, 129, 147, 230, 123, 209, 130, 24, 154, 181, 0, 46
};
// MOHAA Scrambled String table end



static inline void HuffmanPutBitFast(uint8_t* fout, int32_t bitIndex, int32_t bit)
{
	const int32_t byteIndex = bitIndex >> 3;
	const int32_t bitOffset = bitIndex & 7;
	if (bitOffset == 0) // Is this the first bit of a new byte?
	{
		// We don't need to preserve what's already in there,
		// so we can write that byte immediately.
		fout[byteIndex] = (unsigned char)bit;
		return;
	}

	fout[(bitIndex >> 3)] |= bit << (bitIndex & 7);
}

static inline void HuffmanOffsetTransmitFast(uint8_t* fout, int32_t* offset, int32_t ch)
{
	const uint16_t result = HuffmanEncoderTable[ch];
	const uint16_t bitCount = result & 15;
	const uint16_t code = (result >> 4) & 0x7FF;
	const uint32_t bitIndex = *(const uint32_t*)offset;

	int32_t bits = (int32_t)code;
	for (uint32_t i = 0; i < bitCount; ++i)
	{
		HuffmanPutBitFast(fout, bitIndex + i, bits & 1);
		bits >>= 1;
	}

	*offset += (int32_t)bitCount;
}
#endif




static huffman_t		msgHuff;

static qboolean			msgInit = qfalse;
static FILE				*fp=0;

int pcount[256];

/*
==============================================================================

			MESSAGE IO FUNCTIONS

Handles byte ordering and avoids alignment errors
==============================================================================
*/

#ifndef FINAL_BUILD
	int gLastBitIndex = 0;
#endif

int oldsize = 0;

void MSG_initHuffman();

void MSG_Init( msg_t *buf, byte *data, int length ) {
	if (!msgInit) {
		MSG_initHuffman();
	}
	Com_Memset (buf, 0, sizeof(*buf));
	buf->data = data;
	buf->maxsize = length;
	buf->raw = qfalse;
}

void MSG_InitRaw( msg_t *buf, std::vector<byte> *dataRaw) {
	
	Com_Memset (buf, 0, sizeof(*buf));
	buf->dataRaw = dataRaw;
	buf->dataRaw->reserve(MAX_MSGLEN_RAW); // It can become bigger of course but let's just reserve a certain amount so we don't have a slowdown from vector constantly resizing.
	buf->cursize = dataRaw->size();
	buf->maxsize = INT_MAX; // Since its a vector we arent really limited and MAX_MSGLEN would be too short for raw data I think
	buf->raw = qtrue;
}

void MSG_InitOOB( msg_t *buf, byte *data, int length ) {
	if (!msgInit) {
		MSG_initHuffman();
	}
	Com_Memset (buf, 0, sizeof(*buf));
	buf->data = data;
	buf->maxsize = length;
	buf->oob = qtrue;
	buf->raw = qfalse;
}

void MSG_Clear( msg_t *buf ) {
	buf->cursize = 0;
	buf->overflowed = qfalse;
	buf->bit = 0;					//<- in bits
}


void MSG_Bitstream( msg_t *buf ) {
	buf->oob = qfalse;
}

void MSG_BeginReading( msg_t *msg ) {
	msg->readcount = 0;
	msg->bit = 0;
	msg->oob = qfalse;
}

void MSG_BeginReadingOOB( msg_t *msg ) {
	msg->readcount = 0;
	msg->bit = 0;
	msg->oob = qtrue;
}


/*
=============================================================================

bit functions
  
=============================================================================
*/

int	overflows;

// negative bit values include signs
void MSG_WriteBits( msg_t *msg, int value, int bits ) {
	int	i;

	oldsize += bits;

	// this isn't an exact overflow check, but close enough
	if ( msg->maxsize - msg->cursize < 4 ) {
		msg->overflowed = qtrue;
		return;
	}

	if ( bits == 0 || bits < -31 || bits > 32 ) {
		Com_Error( ERR_DROP, "MSG_WriteBits: bad bits %i", bits );
	}

	// check for overflows
	if ( bits != 32 ) {
		if ( bits > 0 ) {
			if ( value > ( ( 1 << bits ) - 1 ) || value < 0 ) {
				overflows++;
#ifndef FINAL_BUILD
//				Com_Printf ("MSG_WriteBits: overflow writing %d in %d bits [index %i]\n", value, bits, gLastBitIndex);
#endif
			}
		} else {
			int	r;

			r = 1 << (bits-1);

			if ( value >  r - 1 || value < -r ) {
				overflows++;
#ifndef FINAL_BUILD
//				Com_Printf ("MSG_WriteBits: overflow writing %d in %d bits [index %i]\n", value, bits, gLastBitIndex);
#endif
			}
		}
	}
	if ( bits < 0 ) {
		bits = -bits;
	}
	// Special handling for new raw type of message (for better compression afterwards)
	if (msg->raw) {
		int bitsToDo = (bits & 7) ? ((bits >> 3) + 1) << 3 : bits;
		for (i = 0; i < bitsToDo; i += 8) {
			msg->dataRaw->push_back(value & 0xff);
			value = (value >> 8);
		}
		msg->cursize = msg->dataRaw->size();
		return;
	}
	else if (msg->oob) {
		if (bits==8) {
			msg->data[msg->cursize] = value;
			msg->cursize += 1;
			msg->bit += 8;
		} else if (bits==16) {
			unsigned short *sp = (unsigned short *)&msg->data[msg->cursize];
			*sp = LittleShort(value);
			msg->cursize += 2;
			msg->bit += 16;
		} else if (bits==32) {
			unsigned int *ip = (unsigned int *)&msg->data[msg->cursize];
			*ip = LittleLong(value);
			msg->cursize += 4;
			msg->bit += 8;
		} else {
			Com_Error(ERR_DROP, "can't read %d bits\n", bits);
		}
	} else {
#ifdef FASTHUFFMAN
		value &= (0xffffffff >> (32 - bits));
		if (bits & 7) {
			int nbits;
			nbits = bits & 7;
			for (i = 0; i < nbits; i++) {
				//Huff_putBit((value & 1), msg->data, &msg->bit);
				HuffmanPutBitFast(msg->data, msg->bit, (value & 1));
				value = (value >> 1);
				msg->bit++;
			}
			bits = bits - nbits;
		}
		if (bits) {
			for (i = 0; i < bits; i += 8) {
#ifdef _NEWHUFFTABLE_
				fwrite(&value, 1, 1, fp);
#endif // _NEWHUFFTABLE_
				//Huff_offsetTransmit(&msgHuff.compressor, (value & 0xff), msg->data, &msg->bit);
				HuffmanOffsetTransmitFast(msg->data, &msg->bit, (value & 0xff));
				value = (value >> 8);
			}
		}
		msg->cursize = (msg->bit >> 3) + 1;
#else
		value &= (0xffffffff>>(32-bits));
		if (bits&7) {
			int nbits;
			nbits = bits&7;
			for(i=0;i<nbits;i++) {
				Huff_putBit((value&1), msg->data, &msg->bit);
				value = (value>>1);
			}
			bits = bits - nbits;
		}
		if (bits) {
			for(i=0;i<bits;i+=8) {
#ifdef _NEWHUFFTABLE_
				fwrite(&value, 1, 1, fp);
#endif // _NEWHUFFTABLE_
				Huff_offsetTransmit (&msgHuff.compressor, (value&0xff), msg->data, &msg->bit);
				value = (value>>8);
			}
		}
		msg->cursize = (msg->bit>>3)+1;
#endif
	}
}

#ifdef MSG_READBITS_TRANSCODE
msg_t* transcodeTargetMsg = NULL;
#endif

int MSG_ReadBits( msg_t *msg, int bits ) {
	int			value;
	int			get;
	qboolean	sgn;
	int			i, nbits;
	value = 0;
#ifdef MSG_READBITS_TRANSCODE
	int originalBits = bits;
#endif

	if ( bits < 0 ) {
		bits = -bits;
		sgn = qtrue;
	} else {
		sgn = qfalse;
	}

	if (msg->raw) {
		int bitsToDo = (bits & 7) ? ((bits >> 3) + 1) << 3 : bits;
		for (i = 0; i < bitsToDo; i += 8) {
			get = (*msg->dataRaw)[msg->readcount + (i >> 3)];
			value |= (get << i);
		}
		msg->readcount += bitsToDo >> 3;
	}
	else if (msg->oob) {
		if (bits==8) {
			value = msg->data[msg->readcount];
			msg->readcount += 1;
			msg->bit += 8;
		} else if (bits==16) {
			unsigned short *sp = (unsigned short *)&msg->data[msg->readcount];
			value = LittleShort(*sp);
			msg->readcount += 2;
			msg->bit += 16;
		} else if (bits==32) {
			unsigned int *ip = (unsigned int *)&msg->data[msg->readcount];
			value = LittleLong(*ip);
			msg->readcount += 4;
			msg->bit += 32;
		} else {
			Com_Error(ERR_DROP, "can't read %d bits\n", bits);
		}
	} else {

// Fast Huffman decoder from:
// from: https://github.com/mightycow/uberdemotools/commit/685b132abc4803f4c813fa07928cd9a4099e5d59
#ifdef FASTHUFFMAN
		const uint8_t* const bufferData = msg->data;
		const int32_t nbits = bits & 7;
		int32_t bitIndex = msg->bit;
		if (nbits)
		{
			const int16_t allBits = *(const int16_t*)(bufferData + (bitIndex >> 3)) >> (bitIndex & 7);
			value = allBits & ((1 << nbits) - 1);
			bitIndex += nbits;
			bits -= nbits;
		}

		if (bits)
		{
			for (int32_t i = 0; i < bits; i += 8)
			{
				const uint16_t code = ((*(const uint32_t*)(bufferData + (bitIndex >> 3))) >> ((uint32_t)bitIndex & 7)) & 0x7FF;
				const uint16_t entry = HuffmanDecoderTable[code];
				value |= (int32_t(entry & 0xFF) << (i + nbits));
				bitIndex += int32_t(entry >> 8);
			}
		}

		msg->bit = bitIndex;
		msg->readcount = (bitIndex >> 3) + 1;
#else
		nbits = 0;
		if (bits&7) {
			nbits = bits&7;
			for(i=0;i<nbits;i++) {
				value |= (Huff_getBit(msg->data, &msg->bit)<<i);
			}
			bits = bits - nbits;
		}
		if (bits) {
			for(i=0;i<bits;i+=8) {
				Huff_offsetReceive (msgHuff.decompressor.tree, &get, msg->data, &msg->bit);
#ifdef _NEWHUFFTABLE_
				fwrite(&get, 1, 1, fp);
#endif // _NEWHUFFTABLE_
				value |= (get<<(i+nbits));
			}
		}
		msg->readcount = (msg->bit>>3)+1;
#endif
	}
	if ( sgn ) {
		if ( value & ( 1 << ( bits - 1 ) ) ) {
			value |= -1 ^ ( ( 1 << bits ) - 1 );
		}
	}

#ifdef MSG_READBITS_TRANSCODE // Write everything we read to the target message too.
	if (transcodeTargetMsg) {
		MSG_WriteBits(transcodeTargetMsg, value, originalBits);
	}
#endif

	return value;
}



//================================================================================

//
// writing functions
//

void MSG_WriteChar( msg_t *sb, int c ) {
#ifdef PARANOID
	if (c < -128 || c > 127)
		Com_Error (ERR_FATAL, "MSG_WriteChar: range error");
#endif

	MSG_WriteBits( sb, c, 8 );
}

void MSG_WriteByte( msg_t *sb, int c ) {
#ifdef PARANOID
	if (c < 0 || c > 255)
		Com_Error (ERR_FATAL, "MSG_WriteByte: range error");
#endif

	MSG_WriteBits( sb, c, 8 );
}

void MSG_WriteData( msg_t *buf, const void *data, int length ) {
	int i;
	for(i=0;i<length;i++) {
		MSG_WriteByte(buf, ((byte *)data)[i]);
	}
}

void MSG_WriteShort( msg_t *sb, int c ) {
#ifdef PARANOID
	if (c < ((short)0x8000) || c > (short)0x7fff)
		Com_Error (ERR_FATAL, "MSG_WriteShort: range error");
#endif

	MSG_WriteBits( sb, c, 16 );
}
void MSG_WriteSShort( msg_t *sb, int c ) {
#ifdef PARANOID
	if (c < ((short)0x8000) || c > (short)0x7fff)
		Com_Error (ERR_FATAL, "MSG_WriteShort: range error");
#endif

	MSG_WriteBits( sb, c, -16 );
}

void MSG_WriteLong( msg_t *sb, int c ) {
	MSG_WriteBits( sb, c, 32 );
}

void MSG_WriteFloat( msg_t *sb, float f ) {
	union {
		float	f;
		int	l;
	} dat;
	
	dat.f = f;
	MSG_WriteBits( sb, dat.l, 32 );
}

void MSG_WriteString( msg_t *sb, const char *s, demoType_t demoType ) {

	bool isMOHAADemoWithScrambledString = demoType == DM3_MOHAA_PROT_15;
	bool isMOHAADemo = demoTypeIsMOHAA(demoType);

	if ( !s ) {
		if (isMOHAADemoWithScrambledString) {
			MSG_WriteByte(sb, StrCharToNetByte[0]);
		}
		else {
			MSG_WriteData(sb, "", 1);
		}
	} else {
		int		l,i;
		char	string[MAX_STRING_CHARS_MAX];

		l = strlen( s );
		if ( l >= (isMOHAADemo ? MAX_STRING_CHARS_MAX : MAX_STRING_CHARS) ) {
			Com_DPrintf( "MSG_WriteString: MAX_STRING_CHARS" );
			if (isMOHAADemoWithScrambledString) {
				MSG_WriteByte(sb, StrCharToNetByte[0]);
			}
			else {
				MSG_WriteData(sb, "", 1);
			}
			return;
		}
		Q_strncpyz( string,sizeof(string), s, sizeof( string ) );

		// get rid of 0xff chars, because old clients don't like them
		for ( i = 0 ; i < l ; i++ ) {
			if (!isMOHAADemo && demoType < DM_91 && ((byte *)string)[i] > 127 ) {
				string[i] = '.';
			}

			// TODO: This is from wolfcamql for quake live ig. Idk.
			// ok to check each byte when parsing UTF-8 since '%' (0x25) isn't a valid UTF-8 byte
			/*if (string[i] == '%') {
				string[i] = '.';
			}*/
			if (isMOHAADemoWithScrambledString) {
				MSG_WriteByte(sb, StrCharToNetByte[string[i]]);
			}
		}

		if (!isMOHAADemoWithScrambledString) {
			MSG_WriteData(sb, string, l + 1);
		}
	}
}

void MSG_WriteBigString( msg_t *sb, const char *s, demoType_t demoType) {

	bool isMOHAADemoWithScrambledString = demoType == DM3_MOHAA_PROT_15;
	bool isMOHAADemo = demoTypeIsMOHAA(demoType);

	if ( !s ) {
		if (isMOHAADemoWithScrambledString) {
			MSG_WriteByte(sb, StrCharToNetByte[0]);
		}
		else {
			MSG_WriteData(sb, "", 1);
		}
	} else {
		int		l,i;
		char	string[BIG_INFO_STRING];

		l = strlen( s );
		if ( l >= BIG_INFO_STRING ) {
			Com_DPrintf( "MSG_WriteString: BIG_INFO_STRING" );
			
		if (isMOHAADemoWithScrambledString) {
			MSG_WriteByte(sb, StrCharToNetByte[0]);
		}
		else {
			MSG_WriteData(sb, "", 1);
		}
			return;
		}
		Q_strncpyz( string,sizeof(string), s, sizeof( string ) );

		// get rid of 0xff chars, because old clients don't like them
		for ( i = 0 ; i < l ; i++ ) {
			if (!isMOHAADemo && demoType < DM_91 && ((byte *)string)[i] > 127 ) {
				string[i] = '.';
			}

			// TODO: This is from wolfcamql for quake live ig. Idk.
			// ok to check each byte when parsing UTF-8 since '%' (0x25) isn't a valid UTF-8 byte
			/*if (string[i] == '%') {
				string[i] = '.';
			}*/

			if (isMOHAADemoWithScrambledString) {
				MSG_WriteByte(sb, StrCharToNetByte[string[i]]);
			}
		}

		if (!isMOHAADemoWithScrambledString) {
			MSG_WriteData(sb, string, l + 1);
		}
	}
}

void MSG_WriteAngle( msg_t *sb, float f ) {
	MSG_WriteByte (sb, (int)(f*256/360) & 255);
}

void MSG_WriteAngle16( msg_t *sb, float f ) {
	MSG_WriteShort (sb, ANGLE2SHORT(f));
}


//============================================================

//
// reading functions
//

// returns -1 if no more characters are available
int MSG_ReadChar (msg_t *msg ) {
	int	c;
	
	c = (signed char)MSG_ReadBits( msg, 8 );
	if ( msg->readcount > msg->cursize ) {
		c = -1;
	}	
	
	return c;
}

int MSG_ReadByte( msg_t *msg ) {
	int	c;
	
	c = (unsigned char)MSG_ReadBits( msg, 8 );
	if ( msg->readcount > msg->cursize ) {
		c = -1;
	}	
	return c;
}

int MSG_ReadShort( msg_t *msg ) {
	int	c;
	
	c = (short)MSG_ReadBits( msg, 16 );
	if ( msg->readcount > msg->cursize ) {
		c = -1;
	}	

	return c;
}

int MSG_ReadSShort( msg_t *msg ) {
	int	c;
	
	c = (short)MSG_ReadBits( msg, -16 );
	if ( msg->readcount > msg->cursize ) {
		c = -1;
	}	

	return c;
}

int MSG_ReadLong( msg_t *msg ) {
	int	c;
	
	c = MSG_ReadBits( msg, 32 );
	if ( msg->readcount > msg->cursize ) {
		c = -1;
	}	
	
	return c;
}

float MSG_ReadFloat( msg_t *msg ) {
	union {
		byte	b[4];
		float	f;
		int	l;
	} dat;
	
	dat.l = MSG_ReadBits( msg, 32 );
	if ( msg->readcount > msg->cursize ) {
		dat.f = -1;
	}	
	
	return dat.f;	
}

char *MSG_ReadString( msg_t *msg, demoType_t demoType ) {
	static char	string[MAX_STRING_CHARS_MAX];
	int		l,c;

	bool isMOHAADemo = demoTypeIsMOHAA(demoType);
	bool isMOHAADemoWithScrambledString = demoType == DM3_MOHAA_PROT_15;

	l = 0;
	do {
		c = MSG_ReadByte(msg);		// use ReadByte so -1 is out of bounds
		if ( c == -1 ) {
			break;
		}

		if (isMOHAADemoWithScrambledString) {
			c = NetByteToStrChar[c];
		}

		if ( c == 0 ) {
			break;
		}
		// translate all fmt spec to avoid crash bugs
		if ( c == '%' ) {
			c = '.';
		}
		// don't allow higher ascii values
		if ( c > 127 && demoType < DM_91 && !isMOHAADemo) { // Quake live stuff. Quake live DM_91 supports UTF-8? Also MOHAA doesn't "dot" the rest
			c = '.';
		}

		string[l] = c;
		l++;
	} while (l <= sizeof(string)-1);
	
	// some bonus protection, shouldn't occur cause server doesn't write such things
	if (l <= sizeof(string)-1)
	{
		string[l] = 0;
	}
	else
	{
		string[sizeof(string)-1] = 0;
	}
	
	return string;
}

char *MSG_ReadBigString( msg_t *msg, demoType_t demoType) {
	static char	string[BIG_INFO_STRING];
	int		l,c;
	
	bool isMOHAADemoWithScrambledString = demoType == DM3_MOHAA_PROT_15;

	l = 0;
	do {
		c = MSG_ReadByte(msg);		// use ReadByte so -1 is out of bounds
		if ( c == -1) {
			break;
		}

		if (isMOHAADemoWithScrambledString) {
			c = NetByteToStrChar[c];
		}

		if (c == 0) {
			break;
		}

		// translate all fmt spec to avoid crash bugs
		if ( c == '%' ) {
			c = '.';
		}

		string[l] = c;
		l++;
	} while (l < sizeof(string)-1);
	
	string[l] = 0;
	
	return string;
}

char *MSG_ReadStringLine( msg_t *msg/*, demoType_t demoType*/) {
	static char	string[MAX_STRING_CHARS_MAX];
	int		l,c;

	l = 0;
	do {
		c = MSG_ReadByte(msg);		// use ReadByte so -1 is out of bounds
		if (c == -1 || c == 0 || c == '\n') {
			break;
		}
		// translate all fmt spec to avoid crash bugs
		if ( c == '%' ) {
			c = '.';
		}

		// UTF-8
		// TODO: wolfcamql wants to do this here but this wasn't here before. Should I add it?
		/*if (demoType < 91) {
			// don't allow higher ascii values
			if (c > 127) {
				c = '.';
			}
		}*/

		string[l] = c;
		l++;
	} while (l < sizeof(string)-1);
	
	string[l] = 0;
	
	return string;
}

float MSG_ReadAngle16( msg_t *msg ) {
	return SHORT2ANGLE(MSG_ReadShort(msg));
}

void MSG_ReadData( msg_t *msg, void *data, int len ) {
	int		i;

	for (i=0 ; i<len ; i++) {
		((byte *)data)[i] = MSG_ReadByte (msg);
	}
}


/*
=============================================================================

delta functions
  
=============================================================================
*/

//extern cvar_t *cl_shownet;

#define	LOG(x) if( cl_shownet->integer == 4 ) { Com_Printf("%s ", x ); };

void MSG_WriteDelta( msg_t *msg, int oldV, int newV, int bits ) {
	if ( oldV == newV ) {
		MSG_WriteBits( msg, 0, 1 );
		return;
	}
	MSG_WriteBits( msg, 1, 1 );
	MSG_WriteBits( msg, newV, bits );
}

int	MSG_ReadDelta( msg_t *msg, int oldV, int bits ) {
	if ( MSG_ReadBits( msg, 1 ) ) {
		return MSG_ReadBits( msg, bits );
	}
	return oldV;
}

void MSG_WriteDeltaFloat( msg_t *msg, float oldV, float newV ) {
	if ( oldV == newV ) {
		MSG_WriteBits( msg, 0, 1 );
		return;
	}
	MSG_WriteBits( msg, 1, 1 );
	MSG_WriteBits( msg, *(int *)&newV, 32 );
}

float MSG_ReadDeltaFloat( msg_t *msg, float oldV ) {
	if ( MSG_ReadBits( msg, 1 ) ) {
		float	newV;

		*(int *)&newV = MSG_ReadBits( msg, 32 );
		return newV;
	}
	return oldV;
}

/*
=============================================================================

delta functions with keys
  
=============================================================================
*/

uint32_t kbitmask[32] = {
	0x00000001, 0x00000003, 0x00000007, 0x0000000F,
	0x0000001F,	0x0000003F,	0x0000007F,	0x000000FF,
	0x000001FF,	0x000003FF,	0x000007FF,	0x00000FFF,
	0x00001FFF,	0x00003FFF,	0x00007FFF,	0x0000FFFF,
	0x0001FFFF,	0x0003FFFF,	0x0007FFFF,	0x000FFFFF,
	0x001FFFFf,	0x003FFFFF,	0x007FFFFF,	0x00FFFFFF,
	0x01FFFFFF,	0x03FFFFFF,	0x07FFFFFF,	0x0FFFFFFF,
	0x1FFFFFFF,	0x3FFFFFFF,	0x7FFFFFFF,	0xFFFFFFFF,
};

void MSG_WriteDeltaKey( msg_t *msg, int key, int oldV, int newV, int bits ) 
{
	if ( oldV == newV ) 
	{
		MSG_WriteBits( msg, 0, 1 );
		return;
	}
	MSG_WriteBits( msg, 1, 1 );
	MSG_WriteBits( msg, (newV ^ key) & ((1 << bits) - 1), bits );
}

int	MSG_ReadDeltaKey( msg_t *msg, int key, int oldV, int bits ) {
	if ( MSG_ReadBits( msg, 1 ) ) {
		return MSG_ReadBits( msg, bits ) ^ (key & kbitmask[bits]);
	}
	return oldV;
}

void MSG_WriteDeltaKeyFloat( msg_t *msg, int key, float oldV, float newV ) {
	if ( oldV == newV ) {
		MSG_WriteBits( msg, 0, 1 );
		return;
	}
	MSG_WriteBits( msg, 1, 1 );
	MSG_WriteBits( msg, (*(int *)&newV) ^ key, 32 );
}

float MSG_ReadDeltaKeyFloat( msg_t *msg, int key, float oldV ) {
	if ( MSG_ReadBits( msg, 1 ) ) {
		float	newV;

		*(int *)&newV = MSG_ReadBits( msg, 32 ) ^ key;
		return newV;
	}
	return oldV;
}


/*
============================================================================

usercmd_t communication

============================================================================
*/

// ms is allways sent, the others are optional
#define	CM_ANGLE1 	(1<<0)
#define	CM_ANGLE2 	(1<<1)
#define	CM_ANGLE3 	(1<<2)
#define	CM_FORWARD	(1<<3)
#define	CM_SIDE		(1<<4)
#define	CM_UP		(1<<5)
#define	CM_BUTTONS	(1<<6)
#define CM_WEAPON	(1<<7)
//rww - these are new
#define CM_FORCE	(1<<8)
#define CM_INVEN	(1<<9)

/*
=====================
MSG_WriteDeltaUsercmd
=====================
*/
void MSG_WriteDeltaUsercmd( msg_t *msg, usercmd_t *from, usercmd_t *to ) {
	if ( to->serverTime - from->serverTime < 256 ) {
		MSG_WriteBits( msg, 1, 1 );
		MSG_WriteBits( msg, to->serverTime - from->serverTime, 8 );
	} else {
		MSG_WriteBits( msg, 0, 1 );
		MSG_WriteBits( msg, to->serverTime, 32 );
	}
	MSG_WriteDelta( msg, from->angles[0], to->angles[0], 16 );
	MSG_WriteDelta( msg, from->angles[1], to->angles[1], 16 );
	MSG_WriteDelta( msg, from->angles[2], to->angles[2], 16 );
	MSG_WriteDelta( msg, from->forwardmove, to->forwardmove, 8 );
	MSG_WriteDelta( msg, from->rightmove, to->rightmove, 8 );
	MSG_WriteDelta( msg, from->upmove, to->upmove, 8 );
	MSG_WriteDelta( msg, from->buttons, to->buttons, 16 );
	MSG_WriteDelta( msg, from->weapon, to->weapon, 8 );

	MSG_WriteDelta( msg, from->forcesel, to->forcesel, 8 );
	MSG_WriteDelta( msg, from->invensel, to->invensel, 8 );

	MSG_WriteDelta( msg, from->generic_cmd, to->generic_cmd, 8 );
}


/*
=====================
MSG_ReadDeltaUsercmd
=====================
*/
void MSG_ReadDeltaUsercmd( msg_t *msg, usercmd_t *from, usercmd_t *to ) {
	if ( MSG_ReadBits( msg, 1 ) ) {
		to->serverTime = from->serverTime + MSG_ReadBits( msg, 8 );
	} else {
		to->serverTime = MSG_ReadBits( msg, 32 );
	}
	to->angles[0] = MSG_ReadDelta( msg, from->angles[0], 16);
	to->angles[1] = MSG_ReadDelta( msg, from->angles[1], 16);
	to->angles[2] = MSG_ReadDelta( msg, from->angles[2], 16);
	to->forwardmove = MSG_ReadDelta( msg, from->forwardmove, 8);
	to->rightmove = MSG_ReadDelta( msg, from->rightmove, 8);
	to->upmove = MSG_ReadDelta( msg, from->upmove, 8);
	to->buttons = MSG_ReadDelta( msg, from->buttons, 16);
	to->weapon = MSG_ReadDelta( msg, from->weapon, 8);

	to->forcesel = MSG_ReadDelta( msg, from->forcesel, 8);
	to->invensel = MSG_ReadDelta( msg, from->invensel, 8);

	to->generic_cmd = MSG_ReadDelta( msg, from->generic_cmd, 8);
}

/*
=====================
MSG_WriteDeltaUsercmd
=====================
*/
void MSG_WriteDeltaUsercmdKey( msg_t *msg, int key, usercmd_t *from, usercmd_t *to ) {
	if ( to->serverTime - from->serverTime < 256 ) {
		MSG_WriteBits( msg, 1, 1 );
		MSG_WriteBits( msg, to->serverTime - from->serverTime, 8 );
	} else {
		MSG_WriteBits( msg, 0, 1 );
		MSG_WriteBits( msg, to->serverTime, 32 );
	}
	if (from->angles[0] == to->angles[0] &&
		from->angles[1] == to->angles[1] &&
		from->angles[2] == to->angles[2] &&
		from->forwardmove == to->forwardmove &&
		from->rightmove == to->rightmove &&
		from->upmove == to->upmove &&
		from->buttons == to->buttons &&
		from->weapon == to->weapon &&
		from->forcesel == to->forcesel &&
		from->invensel == to->invensel &&
		from->generic_cmd == to->generic_cmd) {
			MSG_WriteBits( msg, 0, 1 );				// no change
			oldsize += 7;
			return;
	}
	key ^= to->serverTime;
	MSG_WriteBits( msg, 1, 1 );
	MSG_WriteDeltaKey( msg, key, from->angles[0], to->angles[0], 16 );
	MSG_WriteDeltaKey( msg, key, from->angles[1], to->angles[1], 16 );
	MSG_WriteDeltaKey( msg, key, from->angles[2], to->angles[2], 16 );
	MSG_WriteDeltaKey( msg, key, from->forwardmove, to->forwardmove, 8 );
	MSG_WriteDeltaKey( msg, key, from->rightmove, to->rightmove, 8 );
	MSG_WriteDeltaKey( msg, key, from->upmove, to->upmove, 8 );
	MSG_WriteDeltaKey( msg, key, from->buttons, to->buttons, 16 );
	MSG_WriteDeltaKey( msg, key, from->weapon, to->weapon, 8 );

	MSG_WriteDeltaKey( msg, key, from->forcesel, to->forcesel, 8 );
	MSG_WriteDeltaKey( msg, key, from->invensel, to->invensel, 8 );

	MSG_WriteDeltaKey( msg, key, from->generic_cmd, to->generic_cmd, 8 );
}


/*
=====================
MSG_ReadDeltaUsercmd
=====================
*/
void MSG_ReadDeltaUsercmdKey( msg_t *msg, int key, usercmd_t *from, usercmd_t *to ) {
	if ( MSG_ReadBits( msg, 1 ) ) {
		to->serverTime = from->serverTime + MSG_ReadBits( msg, 8 );
	} else {
		to->serverTime = MSG_ReadBits( msg, 32 );
	}
	if ( MSG_ReadBits( msg, 1 ) ) {
		key ^= to->serverTime;
		to->angles[0] = MSG_ReadDeltaKey( msg, key, from->angles[0], 16);
		to->angles[1] = MSG_ReadDeltaKey( msg, key, from->angles[1], 16);
		to->angles[2] = MSG_ReadDeltaKey( msg, key, from->angles[2], 16);
		to->forwardmove = MSG_ReadDeltaKey( msg, key, from->forwardmove, 8);
		to->rightmove = MSG_ReadDeltaKey( msg, key, from->rightmove, 8);
		to->upmove = MSG_ReadDeltaKey( msg, key, from->upmove, 8);
		to->buttons = MSG_ReadDeltaKey( msg, key, from->buttons, 16);
		to->weapon = MSG_ReadDeltaKey( msg, key, from->weapon, 8);

		to->forcesel = MSG_ReadDeltaKey( msg, key, from->forcesel, 8);
		to->invensel = MSG_ReadDeltaKey( msg, key, from->invensel, 8);

		to->generic_cmd = MSG_ReadDeltaKey( msg, key, from->generic_cmd, 8);
	} else {
		to->angles[0] = from->angles[0];
		to->angles[1] = from->angles[1];
		to->angles[2] = from->angles[2];
		to->forwardmove = from->forwardmove;
		to->rightmove = from->rightmove;
		to->upmove = from->upmove;
		to->buttons = from->buttons;
		to->weapon = from->weapon;

		to->forcesel = from->forcesel;
		to->invensel = from->invensel;

		to->generic_cmd = from->generic_cmd;
	}
}

/*
=============================================================================

entityState_t communication
  
=============================================================================
*/

/*
=================
MSG_ReportChangeVectors_f

Prints out a table from the current statistics for copying to code
=================
*/
void MSG_ReportChangeVectors_f( void ) {
	int i;
	for(i=0;i<256;i++) {
		if (pcount[i]) {
			Com_Printf("%d used %d\n", i, pcount[i]);
		}
	}
}



// if (int)f == f and (int)f + ( 1<<(FLOAT_INT_BITS-1) ) < ( 1 << FLOAT_INT_BITS )
// the float will be sent with FLOAT_INT_BITS, otherwise all 32 bits will be sent
#define	FLOAT_INT_BITS	13
#define	FLOAT_INT_BIAS	(1<<(FLOAT_INT_BITS-1))




//
//
//
// MOHAA functions start
//
//


static constexpr int MAX_PACKED_COORD = 65536;// TA stuff
static constexpr int MAX_PACKED_COORD_HALF = MAX_PACKED_COORD / 2;// TA stuff
static constexpr int MAX_PACKED_COORD_EXTRA = 262144;// TA stuff
static constexpr int MAX_PACKED_COORD_EXTRA_HALF = MAX_PACKED_COORD_EXTRA / 2;// TA stuff


float MSG_ReadCoord(msg_t* msg) {
	float	sign = 1.0f;
	int		read;
	float	rtn;

	read = MSG_ReadBits(msg, 19);
	if (read & 262144)	// the 19th bit is the sign
		sign = -1.0f;
	read &= ~262144; //  uint=4294705151
	rtn = sign * read / 16.0f;

	return rtn;
}

void MSG_ReadDir(msg_t* msg, vec3_t dir) {
	int b;

	b = MSG_ReadByte(msg);
	ByteToDir(b, dir);
}

float MSG_ReadServerFrameTime_ver_15(msg_t* msg) {
	return MSG_ReadFloat(msg);
}


float MSG_ReadServerFrameTime_ver_6(msg_t* msg, clientActive_t* clCut) {

	int stringOffset = demo.cut.Cl.gameState.stringOffsets[CS_SERVERINFO]; // OpenMohaa has CS_SYSTEMINFO but sv_fps is in CS_SERVERINFO
	int maxLength = sizeof(demo.cut.Cl.gameState.stringData) - stringOffset;
	return 1.f / atof(Info_ValueForKey(clCut->gameState.stringData + clCut->gameState.stringOffsets[CS_SERVERINFO], maxLength, "sv_fps")); // OpenMohaa has CS_SYSTEMINFO but sv_fps is in CS_SERVERINFO
}

float MSG_ReadServerFrameTime(msg_t* msg, demoType_t demoType, clientActive_t* clCut, qboolean forceConfigStringMethod) {
	if (demoType==DM3_MOHAA_PROT_15 && !forceConfigStringMethod) {
		return MSG_ReadServerFrameTime_ver_15(msg);
	}
	else {
		// smaller below version 15
		return MSG_ReadServerFrameTime_ver_6(msg, clCut);
	}
}

void MSG_GetNullEntityState(entityState_t* nullState) {

	Com_Memset(nullState, 0, sizeof(entityState_t));
	nullState->alpha = 1.0f;
	nullState->scale = 1.0f;
	nullState->parent = ENTITYNUM_NONE;
	nullState->constantLight = 0xffffff;
	nullState->renderfx = 16;
	nullState->bone_tag[4] = -1;
	nullState->bone_tag[3] = -1;
	nullState->bone_tag[2] = -1;
	nullState->bone_tag[1] = -1;
	nullState->bone_tag[0] = -1;
}

float MSG_UnpackAngle(int value, int bits)
{
	int maxValue;
	float neg;
	float calc;

	neg = 1.f;
	if (bits < 0)
	{
		bits = -1 - bits;
		maxValue = 1 << bits;
		if ((maxValue & value) != 0)
		{
			neg = -1.f;
			value &= ~maxValue;
		}
	}

	switch (bits)
	{
	case 8:
		return neg * 360.f / 256.f;
	case 12:
		return neg * value * 360.f / 4096.f;
	case 16:
		calc = value * 360.f / 65536.f;
		break;
	default:
		calc = 360.f / (1 << bits) * value;
		break;
	}
	return neg * calc;
}

float MSG_UnpackAnimTime(int packed)
{
	return packed / 100.f;
}

float MSG_UnpackAnimWeight(int result, int bits)
{
	const int32_t max = (1 << bits) - 1;
	const float tmp = (float)result / (float)max;

	if (tmp < 0.0f) return 0.f;
	else if (tmp > 1.0f) return 1.f;
	else return tmp;
}

float MSG_UnpackScale(int packed)
{
	return packed / 100.f;
}

float MSG_UnpackAlpha(int packed, int bits)
{
	return (float)packed / (float)((1 << bits) - 1);
}

float MSG_UnpackCoord(int packed, int bits)
{
	return (float)(packed - MAX_PACKED_COORD_HALF) / 4.f;
}

float MSG_UnpackCoordExtra(int packed, int bits)
{
	return (float)(packed - MAX_PACKED_COORD_EXTRA_HALF) / 16.f;
}

void MSG_WriteDeltaCoord(msg_t* msg, int from, int to)
{
	int delta = to - from;
	int deltaAbs = abs(delta);

	if (deltaAbs <= 0 || deltaAbs > 128)
	{
		// high delta, direct value
		MSG_WriteBits(msg, 0, 1);
		MSG_WriteBits(msg, to, 16);
	}
	else
	{
		MSG_WriteBits(msg, 1, 1);

		if (delta < 0) {
			MSG_WriteBits(msg, 1 + ((deltaAbs - 1) << 1), 8);
		}
		else {
			MSG_WriteBits(msg, (deltaAbs - 1) << 1, 8);
		}
	}
}

int MSG_ReadDeltaCoord(msg_t* msg, int from)
{
	int value;
	int delta;

	if (!MSG_ReadBits(msg, 1))
	{
		// no delta
		return MSG_ReadBits(msg, 16);
	}

	value = MSG_ReadBits(msg, 8);
	delta = (value >> 1) + 1;

	if ((value & 1) != 0) {
		delta = -delta;
	}

	return delta + from;
}

void MSG_WriteDeltaCoordExtra(msg_t* msg, int from, int to)
{
	int delta = to - from;
	int deltaAbs = abs(delta);

	if (deltaAbs <= 0 || deltaAbs > 512)
	{
		// high delta, direct value
		MSG_WriteBits(msg, 0, 1);
		MSG_WriteBits(msg, to, 18);
	}
	else
	{
		MSG_WriteBits(msg, 1, 1);

		if (delta < 0) {
			MSG_WriteBits(msg, 1 + ((deltaAbs - 1) << 1), 10);
		}
		else {
			MSG_WriteBits(msg, (deltaAbs - 1) << 1, 10);
		}
	}
}

int MSG_ReadDeltaCoordExtra(msg_t* msg, int from)
{
	int value;
	int delta;

	if (!MSG_ReadBits(msg, 1))
	{
		// no delta
		return MSG_ReadBits(msg, 18);
	}

	value = MSG_ReadBits(msg, 10);
	delta = (value >> 1) + 1;

	if ((value & 1) != 0) {
		delta = -delta;
	}

	return delta + from;
}

void MSG_ReadRegular_ver_15(msg_t* sb, int bits, void* toF)
{
	if (bits == 0)
	{
		// float
		if (!MSG_ReadBits(sb, 1)) {
			// float
			*(float*)toF = 0.0f;
		}
		else
		{
			if (!MSG_ReadBits(sb, 1))
			{
				// integral float
				*(float*)toF = MSG_ReadBits(sb, -FLOAT_INT_BITS);
			}
			else
			{
				// full floating point value
				unsigned int v = MSG_ReadBits(sb, 32);
				if (v & 1) {
					*(int*)toF = ((v + 0x7A000000) >> 1) | 0x80000000;
				}
				else {
					*(int*)toF = (v + 0x7A000000) >> 1;
				}
			}
		}
	}
	else
	{
		if (MSG_ReadBits(sb, 1)) {
			*(int*)toF = MSG_ReadBits(sb, bits);
		}
		else {
			*(int*)toF = 0;
		}
	}
}

void MSG_ReadRegularSimple_ver_15(msg_t* sb, int bits, void* toF)
{
	MSG_ReadRegular_ver_15(sb, bits, toF);
}

void MSG_WriteRegular_ver_15(msg_t* sb, int bits, const void* toF)
{
	float fullFloat;
	int trunc;

	if (bits == 0) {
		// float
		fullFloat = *(float*)toF;
		trunc = (int)fullFloat;

		if (fullFloat == 0.0f) {
			MSG_WriteBits(sb, 0, 1);
			oldsize += FLOAT_INT_BITS;
		}
		else {
			MSG_WriteBits(sb, 1, 1);
			if (trunc == fullFloat && trunc >= -4096 && trunc < 4096) {
				// send as small integer
				MSG_WriteBits(sb, 0, 1);
				MSG_WriteBits(sb, trunc, -FLOAT_INT_BITS);
			}
			else {
				int newvalue = *(int*)toF * 2 - 0x7A000000;
				if (*(int*)toF < 0) {
					newvalue |= 1;
				}
				MSG_WriteBits(sb, 1, 1);
				// send as full floating point value
				MSG_WriteBits(sb, newvalue, 32);
			}
		}
	}
	else {
		if (!*(int*)toF) {
			MSG_WriteBits(sb, 0, 1);
		}
		else {
			MSG_WriteBits(sb, 1, 1);
			// integer
			MSG_WriteBits(sb, *(int*)toF, bits);
		}
	}
}

void MSG_WriteRegularSimple_ver_15(msg_t* sb, int bits, const void* toF)
{
	MSG_WriteRegular_ver_15(sb, bits, toF);
}

void MSG_WriteEntityNum_ver_15(msg_t* sb, short number)
{
	// protocols version 15 and above adds 1 to the entity number
	MSG_WriteBits(sb, (number + 1) % MAX_GENTITIES, GENTITYNUM_BITS);
}

unsigned short MSG_ReadEntityNum_ver_15(msg_t* sb)
{
	return (unsigned short)(MSG_ReadBits(sb, GENTITYNUM_BITS) - 1) % MAX_GENTITIES;
}

void MSG_ReadRegular_ver_6(msg_t* sb, int bits, void* toF)
{
	if (bits == 0)
	{
		if (!MSG_ReadBits(sb, 1)) {
			// float
			*(float*)toF = 0.0f;
		}
		else
		{
			if (!MSG_ReadBits(sb, 1)) {
				*(float*)toF = (int)MSG_ReadBits(sb, FLOAT_INT_BITS) - FLOAT_INT_BIAS;
			}
			else
			{
				// full floating point value
				*(float*)toF = MSG_ReadFloat(sb);
			}
		}
	}
	else
	{
		if (MSG_ReadBits(sb, 1)) {
			*(int*)toF = MSG_ReadBits(sb, bits);
		}
		else {
			*(int*)toF = 0;
		}
	}
}

void MSG_ReadRegularSimple_ver_6(msg_t* sb, int bits, void* toF)
{
	if (bits == 0) {
		// float
		if (MSG_ReadBits(sb, 1) == 0) {
			// integral float
			int trunc = MSG_ReadBits(sb, FLOAT_INT_BITS);
			// bias to allow equal parts positive and negative
			trunc -= FLOAT_INT_BIAS;
			*(float*)toF = trunc;
		}
		else {
			// full floating point value
			*(int*)toF = MSG_ReadBits(sb, 32);
		}
	}
	else {
		// integer
		*(int*)toF = MSG_ReadBits(sb, bits);
	}
}

void MSG_WriteRegular_ver_6(msg_t* sb, int bits, const void* toF)
{
	float fullFloat;
	int trunc;

	if (bits == 0) {
		// float
		fullFloat = *(float*)toF;
		trunc = (int)fullFloat;

		if (fullFloat == 0.0f) {
			MSG_WriteBits(sb, 0, 1);
			oldsize += FLOAT_INT_BITS;
		}
		else {
			MSG_WriteBits(sb, 1, 1);
			if (trunc == fullFloat && trunc + FLOAT_INT_BIAS >= 0 &&
				trunc + FLOAT_INT_BIAS < (1 << FLOAT_INT_BITS)) {
				// send as small integer
				MSG_WriteBits(sb, 0, 1);
				MSG_WriteBits(sb, trunc + FLOAT_INT_BIAS, -FLOAT_INT_BITS);
			}
			else {
				// send as full floating point value
				MSG_WriteBits(sb, 1, 1);
				MSG_WriteBits(sb, *(int*)toF, 32);
			}
		}
	}
	else {
		if (!*(int*)toF) {
			MSG_WriteBits(sb, 0, 1);
		}
		else {
			MSG_WriteBits(sb, 1, 1);
			// integer
			MSG_WriteBits(sb, *(int*)toF, bits);
		}
	}
}

void MSG_WriteRegularSimple_ver_6(msg_t* sb, int bits, const void* toF)
{
	float fullFloat;
	int trunc;

	if (bits == 0) {
		// float
		fullFloat = *(float*)toF;
		trunc = (int)fullFloat;

		if (trunc == fullFloat && trunc + FLOAT_INT_BIAS >= 0 &&
			trunc + FLOAT_INT_BIAS < (1 << FLOAT_INT_BITS)) {
			// send as small integer
			MSG_WriteBits(sb, 0, 1);
			MSG_WriteBits(sb, trunc + FLOAT_INT_BIAS, FLOAT_INT_BITS);
		}
		else {
			// send as full floating point value
			MSG_WriteBits(sb, 1, 1);
			MSG_WriteBits(sb, *(int*)toF, 32);
		}
	}
	else {
		// integer
		MSG_WriteBits(sb, *(int*)toF, bits);
	}
}

void MSG_WriteEntityNum_ver_6(msg_t* sb, short number)
{
	MSG_WriteBits(sb, number % MAX_GENTITIES, GENTITYNUM_BITS);
}

unsigned short MSG_ReadEntityNum_ver_6(msg_t* sb)
{
	return MSG_ReadBits(sb, GENTITYNUM_BITS) % MAX_GENTITIES;
}

void MSG_ReadRegular(msg_t* sb, int bits, void* toF, demoType_t demoType)
{
	if (demoType == DM3_MOHAA_PROT_15) {
		return MSG_ReadRegular_ver_15(sb, bits, toF);
	}
	else {
		return MSG_ReadRegular_ver_6(sb, bits, toF);
	}
}

void MSG_ReadRegularSimple(msg_t* sb, int bits, void* toF, demoType_t demoType)
{
	if (demoType == DM3_MOHAA_PROT_15) {
		return MSG_ReadRegularSimple_ver_15(sb, bits, toF);
	}
	else {
		return MSG_ReadRegularSimple_ver_6(sb, bits, toF);
	}
}

void MSG_WriteRegular(msg_t* sb, int bits, const void* toF, demoType_t demoType)
{
	if (demoType == DM3_MOHAA_PROT_15) {
		return MSG_WriteRegular_ver_15(sb, bits, toF);
	}
	else {
		return MSG_WriteRegular_ver_6(sb, bits, toF);
	}
}

void MSG_WriteRegularSimple(msg_t* sb, int bits, const void* toF, demoType_t demoType)
{
	if (demoType == DM3_MOHAA_PROT_15) {
		return MSG_WriteRegularSimple_ver_15(sb, bits, toF);
	}
	else {
		return MSG_WriteRegularSimple_ver_6(sb, bits, toF);
	}
}

void MSG_WriteEntityNum(msg_t* sb, short number, demoType_t demoType)
{
	if (demoType == DM3_MOHAA_PROT_15) {
		return MSG_WriteEntityNum_ver_15(sb, number);
	}
	else {
		return MSG_WriteEntityNum_ver_6(sb, number);
	}
}

unsigned short MSG_ReadEntityNum(msg_t* sb, demoType_t demoType)
{
	if (demoType == DM3_MOHAA_PROT_15) {
		return MSG_ReadEntityNum_ver_15(sb);
	}
	else {
		return MSG_ReadEntityNum_ver_6(sb);
	}
}


int MSG_PackAngle(float angle, int bits)
{
	int bit;
	float calc;

	bit = 0;
	if (bits < 0)
	{
		bits = ~bits;
		if (angle < 0.0)
		{
			angle = -angle;
			bit = 1 << bits;
		}
	}

	switch (bits)
	{
	case 8:
		calc = angle * 256.f / 360.f;
		return bit | ((int)calc & 0xFF);
	case 12:
		calc = angle * 4096.f / 360.f;
		return bit | ((int)calc & 0xFFF);
	case 16:
		calc = angle * 65536.f / 360.f;
		return bit | ((int)calc & 0xFFFF);
	default:
		calc = (1 << bits) * angle / 360.f;
		return bit | ((int)calc & ((1 << bits) - 1));
	}
}

int MSG_PackAnimTime(float time, int bits)
{
	int maxValue;
	int packed;

	maxValue = (1 << bits) - 1;
	packed = time * 100.f;
	if (packed >= 0)
	{
		if (packed > maxValue) {
			packed = maxValue;
		}
	}
	else
	{
		packed = 0;
	}

	//timestats[packed]++;

	return packed;
}

int MSG_PackAnimWeight(float weight, int bits)
{
	int maxValue;
	int packed;

	maxValue = (1 << bits) - 1;
	packed = maxValue * weight;
	if (packed >= 0)
	{
		if (packed > maxValue) {
			packed = maxValue;
		}
	}
	else
	{
		packed = 0;
	}

	//weightstats[packed]++;

	return packed;
}

int MSG_PackScale(float scale, int bits)
{
	int maxValue;
	int packed;

	maxValue = (1 << bits) - 1;
	packed = scale * 100.f;
	if (packed >= 0)
	{
		if (packed > maxValue) {
			packed = maxValue;
		}
	}
	else
	{
		packed = 0;
	}

	//scalestats[packed]++;

	return packed;
}

int MSG_PackAlpha(float alpha, int bits)
{
	int maxValue;
	int packed;

	maxValue = (1 << bits) - 1;
	packed = maxValue * alpha;
	if (packed >= 0)
	{
		if (packed > maxValue) {
			packed = maxValue;
		}
	}
	else
	{
		packed = 0;
	}

	//alphastats[packed]++;

	return packed;
}

int MSG_PackCoord(float coord)
{
	unsigned int packed = (unsigned int)round(coord * 4.0 + MAX_PACKED_COORD_HALF);
	//coordstats[packed]++;

	return packed;
}

int MSG_PackCoordExtra(float coord)
{
	unsigned int packed = (unsigned int)round(coord * 16.0 + MAX_PACKED_COORD_EXTRA_HALF);
	if (packed >= MAX_PACKED_COORD_EXTRA) {
		Com_DPrintf("Illegal XYZ coordinates for an entity, information lost in transmission\n");
	}
	else {
		// This check wasn't added in >= 2.0
		// which means a player could crash a server when out of bounds
		//++coordextrastats[packed];
	}

	return packed;
}

float MSG_ReadPackedAngle_ver_15(msg_t* msg, int bits)
{
	int packed = MSG_ReadBits(msg, abs(bits));
	return MSG_UnpackAngle(packed, bits);
}

float MSG_ReadPackedAnimTime_ver_15(msg_t* msg, int bits, float fromValue, float frameTime)
{
	int packed;
	if (!MSG_ReadBits(msg, 1)) {
		return fromValue + frameTime;
	}

	packed = MSG_ReadBits(msg, bits);
	return MSG_UnpackAnimTime(packed);
}

float MSG_ReadPackedAnimWeight_ver_15(msg_t* msg, int bits)
{
	int packed = MSG_ReadBits(msg, bits);
	return MSG_UnpackAnimWeight(packed, bits);
}

float MSG_ReadPackedScale_ver_15(msg_t* msg, int bits)
{
	int packed = MSG_ReadBits(msg, bits);
	return MSG_UnpackScale(packed);
}

float MSG_ReadPackedAlpha_ver_15(msg_t* msg, int bits)
{
	int packed = MSG_ReadBits(msg, bits);
	return MSG_UnpackAlpha(packed, bits);
}

float MSG_ReadPackedCoord_ver_15(msg_t* msg, float fromValue, int bits)
{
	int packedFrom = MSG_PackCoord(fromValue);
	int packedTo = MSG_ReadDeltaCoord(msg, packedFrom);
	return MSG_UnpackCoord(packedTo, bits);
}

float MSG_ReadPackedCoordExtra_ver_15(msg_t* msg, float fromValue, int bits)
{
	int packedFrom = MSG_PackCoordExtra(fromValue);
	int packedTo = MSG_ReadDeltaCoordExtra(msg, packedFrom);
	return MSG_UnpackCoordExtra(packedTo, bits);
}

void MSG_WritePackedAngle_ver_15(msg_t* msg, float value, int bits)
{
	int packed = MSG_PackAngle(value, bits);
	MSG_WriteBits(msg, packed, abs(bits));
}

void MSG_WritePackedAnimTime_ver_15(msg_t* msg, float fromValue, float toValue, float frameTime, int bits)
{
	int packed;

	if (fabs(fromValue - toValue) < frameTime) {
		// below the frame time, don't send
		MSG_WriteBits(msg, 0, 1);
		return;
	}

	MSG_WriteBits(msg, 1, 1);
	packed = MSG_PackAnimTime(toValue, bits);
	MSG_WriteBits(msg, packed, bits);
}

void MSG_WritePackedAnimWeight_ver_15(msg_t* msg, float value, int bits)
{
	int packed = MSG_PackAnimWeight(value, bits);
	MSG_WriteBits(msg, packed, bits);
}

void MSG_WritePackedScale_ver_15(msg_t* msg, float value, int bits)
{
	int packed = MSG_PackScale(value, bits);
	MSG_WriteBits(msg, packed, bits);
}

void MSG_WritePackedAlpha_ver_15(msg_t* msg, float value, int bits)
{
	int packed = MSG_PackAlpha(value, bits);
	MSG_WriteBits(msg, packed, bits);
}

void MSG_WritePackedCoord_ver_15(msg_t* msg, float fromValue, float toValue, int bits)
{
	int packedFrom = MSG_PackCoord(fromValue);
	int packedTo = MSG_PackCoord(toValue);
	MSG_WriteDeltaCoord(msg, packedFrom, packedTo);
}

void MSG_WritePackedCoordExtra_ver_15(msg_t* msg, float fromValue, float toValue, int bits)
{
	int packedFrom = MSG_PackCoordExtra(fromValue);
	int packedTo = MSG_PackCoordExtra(toValue);
	MSG_WriteDeltaCoordExtra(msg, packedFrom, packedTo);
}

qboolean MSG_DeltaNeeded_ver_15(const void* fromField, const void* toField, int fieldType, int bits)
{
	int packedFrom;
	int packedTo;
	int maxValue;

	if (*(int*)fromField == *(int*)toField) {
		return qfalse;
	}

	switch (fieldType)
	{
	case netFieldType_e::regular:
		if (!bits || bits == 32) {
			return qtrue;
		}

		maxValue = (1 << abs(bits)) - 1;
		return (qboolean)(((*(int*)fromField ^ *(int*)toField) & maxValue) != 0);
	case netFieldType_e::angle:
		packedFrom = MSG_PackAngle(*(float*)fromField, bits);
		packedTo = MSG_PackAngle(*(float*)toField, bits);
		return (qboolean)(packedFrom != packedTo);
	case netFieldType_e::animTime:
		packedFrom = MSG_PackAnimTime(*(float*)fromField, bits);
		packedTo = MSG_PackAnimTime(*(float*)toField, bits);
		return (qboolean)(packedFrom != packedTo);
	case netFieldType_e::animWeight:
		packedFrom = MSG_PackAnimWeight(*(float*)fromField, bits);
		packedTo = MSG_PackAnimWeight(*(float*)toField, bits);
		return (qboolean)(packedFrom != packedTo);
	case netFieldType_e::scale:
		packedFrom = MSG_PackScale(*(float*)fromField, bits);
		packedTo = MSG_PackScale(*(float*)toField, bits);
		return (qboolean)(packedFrom != packedTo);
	case netFieldType_e::alpha:
		packedFrom = MSG_PackAlpha(*(float*)fromField, bits);
		packedTo = MSG_PackAlpha(*(float*)toField, bits);
		return (qboolean)(packedFrom != packedTo);
	case netFieldType_e::coord:
		packedFrom = MSG_PackCoord(*(float*)fromField);
		packedTo = MSG_PackCoord(*(float*)toField);
		return (qboolean)(packedFrom != packedTo);
	case netFieldType_e::coordExtra:
		packedFrom = MSG_PackCoordExtra(*(float*)fromField);
		packedTo = MSG_PackCoordExtra(*(float*)toField);
		return (qboolean)(packedFrom != packedTo);
	case netFieldType_e::velocity:
		return qtrue;
	case netFieldType_e::simple:
		return qtrue;
	default:
		return qtrue;
	}
}

float MSG_ReadPackedAngle_ver_6(msg_t* msg, int bits)
{
	int result;
	float tmp = 1.0f;
	if (bits < 0) {
		if (MSG_ReadBits(msg, 1))
			tmp = -1.0f;
		bits = ~bits;
	}

	result = MSG_ReadBits(msg, bits);
	switch (bits)
	{
	case 8:
		return tmp * 360.f / 256.f;
	case 12:
		return tmp * result * 360.f / 4096.f;
	case 16:
		return tmp * result * 360.f / 65536.f;
	default:
		return tmp * 360.f / (1 << bits) * result;
	}
}

float MSG_ReadPackedAnimTime_ver_6(msg_t* msg, int bits, float fromValue, float frameTime)
{
	return MSG_ReadBits(msg, 15) / 100.0f;
}

float MSG_ReadPackedAnimWeight_ver_6(msg_t* msg, int bits)
{
	float tmp = MSG_ReadBits(msg, 8) / 255.0f;
	if (tmp < 0.0f)
		return 0.0f;
	else if (tmp > 1.0f)
		return 1.0f;
	else
		return tmp;
}

float MSG_ReadPackedScale_ver_6(msg_t* msg, int bits)
{
	return MSG_ReadBits(msg, 10) / 100.0f;
}

float MSG_ReadPackedAlpha_ver_6(msg_t* msg, int bits)
{
	float tmp = MSG_ReadBits(msg, 8) / 255.0f;
	if (tmp < 0.0f)
		return 0.0f;
	else if (tmp > 1.0f)
		return 1.0f;
	else
		return tmp;
}

float MSG_ReadPackedCoord_ver_6(msg_t* msg, float fromValue, int bits)
{
	float tmp = 1.0f;
	int value = MSG_ReadBits(msg, 19);
	if (value & 262144) // test for 19th bit
		tmp = -1.0f;
	value &= ~262144;	// remove that bit
	return tmp * value / 16.0f;
}

float MSG_ReadPackedCoordExtra_ver_6(msg_t* msg, float fromValue, int bits)
{
	int packedFrom = MSG_PackCoordExtra(fromValue);
	int packedTo = MSG_ReadDeltaCoordExtra(msg, packedFrom);
	return MSG_UnpackCoordExtra(packedTo, bits);
}


void MSG_WritePackedAngle_ver_6(msg_t* msg, float value, int bits)
{
	// angles, what a mess! it wouldnt surprise me if something goes wrong here ;)

	float tmp = value;

	if (bits < 0) {
		if (tmp < 0.0f) {
			MSG_WriteBits(msg, 1, 1);
			tmp = -tmp;
		}
		else {
			MSG_WriteBits(msg, 0, 1);
		}

		bits = ~bits;
	}
	else {
		bits = bits;
	}

	if (bits == 12) {
		tmp = tmp * 4096.0f / 360.0f;
		MSG_WriteBits(msg, ((int)tmp) & 4095, 12);
	}
	else if (bits == 8) {
		tmp = tmp * 256.0f / 360.0f;
		MSG_WriteBits(msg, ((int)tmp) & 255, 8);
	}
	else if (bits == 16) {
		tmp = tmp * 65536.0f / 360.0f;
		MSG_WriteBits(msg, ((int)tmp) & 65535, 16);
	}
	else {
		tmp = tmp * (1 << (byte)bits) / 360.0f;
		MSG_WriteBits(msg, ((int)tmp) & ((1 << (byte)bits) - 1), bits);
	}
}

void MSG_WritePackedAnimTime_ver_6(msg_t* msg, float fromValue, float toValue, float frameTime, int bits)
{
	int packed = toValue * 100.0f;
	if (packed < 0) {
		packed = 0;
	}
	else if (packed >= (1 << 15)) {
		packed = (1 << 15);
	}

	MSG_WriteBits(msg, packed, 15);
}

void MSG_WritePackedAnimWeight_ver_6(msg_t* msg, float value, int bits)
{
	int packed = (value * 255.0f) + 0.5f;

	if (packed < 0) {
		packed = 0;
	}
	else if (packed > 255) {
		packed = 255;
	}

	MSG_WriteBits(msg, packed, 8);
}

void MSG_WritePackedScale_ver_6(msg_t* msg, float value, int bits)
{
	int packed = value * 100.0f;
	if (packed < 0) {
		packed = 0;
	}
	else if (packed > 1023) {
		packed = 1023;
	}

	MSG_WriteBits(msg, packed, 10);
}

void MSG_WritePackedAlpha_ver_6(msg_t* msg, float value, int bits)
{
	int packed = (value * 255.0f) + 0.5f;

	if (packed < 0) {
		packed = 0;
	}
	else if (packed > 255) {
		packed = 255;
	}

	MSG_WriteBits(msg, packed, 8);
}

void MSG_WritePackedCoord_ver_6(msg_t* msg, float fromValue, float toValue, int bits)
{
	int packed = toValue * 16.0f;

	if (toValue < 0) {
		packed = ((-packed) & 262143) | 262144;
	}
	else {
		packed = packed & 262143;
	}

	MSG_WriteBits(msg, packed, 19);
}

void MSG_WritePackedCoordExtra_ver_6(msg_t* msg, float fromValue, float toValue, int bits) {
	// Don't implement
}

qboolean MSG_DeltaNeeded_ver_6(const void* fromField, const void* toField, int fieldType, int bits)
{
	// Unoptimized in base game
	// Doesn't compare packed values
	return (qboolean)(*(int*)fromField != *(int*)toField);
}

float MSG_ReadPackedAngle(msg_t* msg, int bits, demoType_t demoType) {
	if (demoType == DM3_MOHAA_PROT_15) {
		return MSG_ReadPackedAngle_ver_15(msg, bits);
	}
	else {
		return MSG_ReadPackedAngle_ver_6(msg, bits);
	}
}

float MSG_ReadPackedAnimTime(msg_t* msg, int bits, float fromValue, float frameTime, demoType_t demoType) {
	if (demoType == DM3_MOHAA_PROT_15) {
		return MSG_ReadPackedAnimTime_ver_15(msg, bits, fromValue, frameTime);
	}
	else {
		return MSG_ReadPackedAnimTime_ver_6(msg, bits, fromValue, frameTime);
	}
}

float MSG_ReadPackedAnimWeight(msg_t* msg, int bits, demoType_t demoType) {
	if (demoType == DM3_MOHAA_PROT_15) {
		return MSG_ReadPackedAnimWeight_ver_15(msg, bits);
	}
	else {
		return MSG_ReadPackedAnimWeight_ver_6(msg, bits);
	}
}

float MSG_ReadPackedScale(msg_t* msg, int bits, demoType_t demoType) {
	if (demoType == DM3_MOHAA_PROT_15) {
		return MSG_ReadPackedScale_ver_15(msg, bits);
	}
	else {
		return MSG_ReadPackedScale_ver_6(msg, bits);
	}
}

float MSG_ReadPackedAlpha(msg_t* msg, int bits, demoType_t demoType) {
	if (demoType == DM3_MOHAA_PROT_15) {
		return MSG_ReadPackedAlpha_ver_15(msg, bits);
	}
	else {
		return MSG_ReadPackedAlpha_ver_6(msg, bits);
	}
}

float MSG_ReadPackedCoord(msg_t* msg, float fromValue, int bits, demoType_t demoType) {
	if (demoType == DM3_MOHAA_PROT_15) {
		return MSG_ReadPackedCoord_ver_15(msg, fromValue, bits);
	}
	else {
		return MSG_ReadPackedCoord_ver_6(msg, fromValue, bits);
	}
}

float MSG_ReadPackedCoordExtra(msg_t* msg, float fromValue, int bits, demoType_t demoType)
{
	if (demoType == DM3_MOHAA_PROT_15) {
		return MSG_ReadPackedCoordExtra_ver_15(msg, fromValue, bits);
	}
	else {
		return MSG_ReadPackedCoordExtra_ver_6(msg, fromValue, bits);
	}
}


void MSG_WritePackedAngle(msg_t* msg, float value, int bits, demoType_t demoType) {
	if (demoType == DM3_MOHAA_PROT_15) {
		return MSG_WritePackedAngle_ver_15(msg, value, bits);
	}
	else {
		return MSG_WritePackedAngle_ver_6(msg, value, bits);
	}
}

void MSG_WritePackedAnimTime(msg_t* msg, float fromValue, float toValue, float frameTime, int bits, demoType_t demoType) {
	if (demoType == DM3_MOHAA_PROT_15) {
		return MSG_WritePackedAnimTime_ver_15(msg, fromValue, toValue, frameTime, bits);
	}
	else {
		return MSG_WritePackedAnimTime_ver_6(msg, fromValue, toValue, frameTime, bits);
	}
}

void MSG_WritePackedAnimWeight(msg_t* msg, float value, int bits, demoType_t demoType) {
	if (demoType == DM3_MOHAA_PROT_15) {
		return MSG_WritePackedAnimWeight_ver_15(msg, value, bits);
	}
	else {
		return MSG_WritePackedAnimWeight_ver_6(msg, value, bits);
	}
}

void MSG_WritePackedScale(msg_t* msg, float value, int bits, demoType_t demoType) {
	if (demoType == DM3_MOHAA_PROT_15) {
		return MSG_WritePackedScale_ver_15(msg, value, bits);
	}
	else {
		return MSG_WritePackedScale_ver_6(msg, value, bits);
	}
}

void MSG_WritePackedAlpha(msg_t* msg, float value, int bits, demoType_t demoType) {
	if (demoType == DM3_MOHAA_PROT_15) {
		return MSG_WritePackedAlpha_ver_15(msg, value, bits);
	}
	else {
		return MSG_WritePackedAlpha_ver_6(msg, value, bits);
	}
}

void MSG_WritePackedCoord(msg_t* msg, float fromValue, float toValue, int bits, demoType_t demoType) {
	if (demoType == DM3_MOHAA_PROT_15) {
		return MSG_WritePackedCoord_ver_15(msg, fromValue, toValue, bits);
	}
	else {
		return MSG_WritePackedCoord_ver_6(msg, fromValue, toValue, bits);
	}
}

void MSG_WritePackedCoordExtra(msg_t* msg, float fromValue, float toValue, int bits, demoType_t demoType) {
	if (demoType == DM3_MOHAA_PROT_15) {
		return MSG_WritePackedCoordExtra_ver_15(msg, fromValue, toValue, bits);
	}
	else {
		return MSG_WritePackedCoordExtra_ver_6(msg, fromValue, toValue, bits);
	}
}

qboolean MSG_DeltaNeeded(const void* fromField, const void* toField, int fieldType, int bits)
{
	// Unoptimized in base game
	// Doesn't compare packed values
	return (qboolean)(*(int*)fromField != *(int*)toField);
}

float MSG_ReadPackedVelocity(msg_t* msg, int bits)
{
	float tmp = 1.0f;
	int value = MSG_ReadBits(msg, 17);
	if (value & 65536) // test for 17th bit
		tmp = -1.0f;
	value &= ~65536; // remove that bit
	return tmp * value / 8.0f;
}

int MSG_ReadPackedSimple(msg_t* msg, int fromValue, int bits)
{
	if (!MSG_ReadBits(msg, 1)) {
		return fromValue;
	}

	return MSG_ReadBits(msg, bits);
}

void MSG_WritePackedVelocity(msg_t* msg, float value, int bits)
{
	int32_t packed = (uint32_t)(value * 8.0f);
	if (value < 0) {
		packed = ((-packed) & 65535) | 65536;
	}
	else {
		packed = packed & 65535;
	}

	MSG_WriteBits(msg, packed, 17);
}

void MSG_WritePackedSimple(msg_t* msg, int value, int bits)
{
	byte packed = (byte)value;
	if (!packed) {
		MSG_WriteBits(msg, 0, 1);
	}

	MSG_WriteBits(msg, 1, 1);
	MSG_WriteBits(msg, packed, bits);
}

/*
==================
MSG_WriteSounds

write the sounds to the snapshot...
1:1 translated from assembly code
==================
*/
void MSG_WriteSounds(msg_t* msg, server_sound_t* sounds, int snapshot_number_of_sounds) {

	int		i;

	if (!snapshot_number_of_sounds) {
		MSG_WriteBits(msg, 0, 1);
	}
	else {
		MSG_WriteBits(msg, 1, 1);
		MSG_WriteBits(msg, snapshot_number_of_sounds, 7);

		for (i = 0; i < snapshot_number_of_sounds; i++) {
			if (!sounds[i].stop_flag) {
				MSG_WriteBits(msg, 0, 1);
				MSG_WriteBits(msg, sounds[i].streamed, 1);

				if (sounds[i].origin[0] == 0.0f && sounds[i].origin[1] == 0.0f && sounds[i].origin[2] == 0.0f)
					MSG_WriteBits(msg, 0, 1);
				else {
					MSG_WriteBits(msg, 1, 1);
					MSG_WriteFloat(msg, sounds[i].origin[0]);
					MSG_WriteFloat(msg, sounds[i].origin[1]);
					MSG_WriteFloat(msg, sounds[i].origin[2]);
				}
				MSG_WriteBits(msg, sounds[i].entity_number, 11);
				MSG_WriteBits(msg, sounds[i].channel, 7);
				MSG_WriteBits(msg, sounds[i].sound_index, 9);

				if (sounds[i].volume != -1.0f) {
					MSG_WriteBits(msg, 1, 1);
					MSG_WriteFloat(msg, sounds[i].volume);
				}
				else {
					MSG_WriteBits(msg, 0, 1);
				}

				if (sounds[i].min_dist != -1.0f) {
					MSG_WriteBits(msg, 1, 1);
					MSG_WriteFloat(msg, sounds[i].min_dist);
				}
				else {
					MSG_WriteBits(msg, 0, 1);
				}

				if (sounds[i].pitch != -1.0f) {
					MSG_WriteBits(msg, 1, 1);
					MSG_WriteFloat(msg, sounds[i].pitch);
				}
				else {
					MSG_WriteBits(msg, 0, 1);
				}

				MSG_WriteFloat(msg, sounds[i].maxDist);
			}
			else {
				MSG_WriteBits(msg, 1, 1);
				MSG_WriteBits(msg, sounds[i].entity_number, 10);
				MSG_WriteBits(msg, sounds[i].channel, 7);
			}
		}
	}
}

/*
==================
MSG_ReadSounds

read the sounds from the snapshot...
1:1 translated from assembly code
==================
*/
void MSG_ReadSounds(msg_t* msg, server_sound_t* sounds, int* snapshot_number_of_sounds) {

	int		fubar;
	int		i;

	if (MSG_ReadBits(msg, 1)) {
		fubar = MSG_ReadBits(msg, 7);

		if (fubar <= 64) {
			*snapshot_number_of_sounds = fubar;
			for (i = 0; i < fubar; i++) {
				if (MSG_ReadBits(msg, 1) == 1) {
					sounds[i].entity_number = MSG_ReadBits(msg, 10);
					sounds[i].channel = MSG_ReadBits(msg, 7);
					sounds[i].stop_flag = qtrue; // su44 was here
				}
				else {
					sounds[i].stop_flag = qfalse;
					sounds[i].streamed = (qboolean)MSG_ReadBits(msg, 1);
					if (MSG_ReadBits(msg, 1) == 1) {
						sounds[i].origin[0] = MSG_ReadFloat(msg);
						sounds[i].origin[1] = MSG_ReadFloat(msg);
						sounds[i].origin[2] = MSG_ReadFloat(msg);
					}
					else {
						sounds[i].origin[0] = 0;
						sounds[i].origin[1] = 0;
						sounds[i].origin[2] = 0;
					}
					sounds[i].entity_number = MSG_ReadBits(msg, 11);
					sounds[i].channel = MSG_ReadBits(msg, 7);
					sounds[i].sound_index = MSG_ReadBits(msg, 9);

					if (MSG_ReadBits(msg, 1) == 1) {
						sounds[i].volume = MSG_ReadFloat(msg);
					}
					else {
						sounds[i].volume = -1.0f;
					}

					if (MSG_ReadBits(msg, 1) == 1) {
						sounds[i].min_dist = MSG_ReadFloat(msg);
					}
					else {
						sounds[i].min_dist = -1.0f;
					}

					if (MSG_ReadBits(msg, 1) == 1) {
						sounds[i].pitch = MSG_ReadFloat(msg);
					}
					else {
						sounds[i].pitch = 1.0f; // su44 was here
					}

					sounds[i].maxDist = MSG_ReadFloat(msg);
				}
			}

		}
	}
}


//
// MOHAA functions end
//
//




/*
==================
MSG_WriteDeltaEntity

Writes part of a packetentities message, including the entity number.
Can delta from either a baseline or a previous packet_entity
If to is NULL, a remove entity update will be sent
If force is not set, then nothing at all will be generated if the entity is
identical, under the assumption that the in-order delta code will catch it.
==================
*/
void MSG_WriteDeltaEntity( msg_t *msg, struct entityState_s *from, struct entityState_s *to, 
						   qboolean force, demoType_t demoType, float frameTime) {
	int			i, lc;
	int			numFields;
	const netField_t	*entityStateFieldsHere;
	const netField_t	*field;
	int			trunc;
	float		fullFloat;
	int			*fromF, *toF;
	qboolean	deltasNeeded[146]; // MOHAA

	bool isMOHAADemo = demoTypeIsMOHAA(demoType);

	if (!getEntityStateFields(&entityStateFieldsHere, &numFields, demoType)) {
		throw new std::exception("failed to get entitystatefields for writing");
	}
	//if (demo15detected)
	//	numFields = sizeof(entityStateFields15)/sizeof(entityStateFields15[0]);
	//else
	//	numFields = sizeof(entityStateFields)/sizeof(entityStateFields[0]);

	// all fields should be 32 bits to avoid any compiler packing issues
	// the "number" field is not part of the field list
	// if this assert fails, someone added a field to the entityState_t
	// struct without updating the message fields
	//assert( numFields + 1 == sizeof( *from )/4 ); // This assert isn't helping us anymore because we made the whole thing compatible with JKA so it's never really gonna be right.

	// a NULL to is a delta remove message
	if ( to == NULL ) {
		if ( from == NULL ) {
			return;
		}
		MSG_WriteBits( msg, from->number, GENTITYNUM_BITS );
		MSG_WriteBits( msg, 1, 1 );
		return;
	}

	if ( to->number < 0 || to->number >= MAX_GENTITIES ) {
		Com_Error (ERR_FATAL, "MSG_WriteDeltaEntity: Bad entity number: %i", to->number );
	}

	lc = 0;
	// build the change vector as bytes so it is endien independent
	for ( i = 0, field = entityStateFieldsHere; i < numFields ; i++, field++ ) {
		fromF = (int *)( (byte *)from + field->offset );
		toF = (int *)( (byte *)to + field->offset );
		if (isMOHAADemo) {
			deltasNeeded[i] = MSG_DeltaNeeded(fromF, toF, field->type, field->bits);
			if (deltasNeeded[i]) {
				lc = i + 1;
			}
		}
		else {
			if (*fromF != *toF) {
				lc = i + 1;
			}
		}
	}

	if ( lc == 0 ) {
		// nothing at all changed
		if ( !force ) {
			return;		// nothing at all
		}
		// write two bits for no change
		MSG_WriteBits( msg, to->number, GENTITYNUM_BITS );
		MSG_WriteBits( msg, 0, 1 );		// not removed
		MSG_WriteBits( msg, 0, 1 );		// no delta
		return;
	}

	MSG_WriteBits( msg, demoType == DM3_MOHAA_PROT_15 ? ((to->number + 1) % GENTITYNUM_BITS) : to->number, GENTITYNUM_BITS ); // mohaa extensions apparently add 1 to the value and then subtract it when reading. idfk why.
	MSG_WriteBits( msg, 0, 1 );			// not removed
	MSG_WriteBits( msg, 1, 1 );			// we have a delta

	MSG_WriteByte( msg, lc );	// # of changes

	oldsize += numFields;

	for ( i = 0, field = entityStateFieldsHere; i < lc ; i++, field++ ) {
		fromF = (int *)( (byte *)from + field->offset );
		toF = (int *)( (byte *)to + field->offset );

		if ((!isMOHAADemo && *fromF == *toF) || (isMOHAADemo && !deltasNeeded[i]) ) {
			MSG_WriteBits( msg, 0, 1 );	// no change
			continue;
		}

		MSG_WriteBits( msg, 1, 1 );	// changed

		if (isMOHAADemo) {
			switch (field->type) {
				// normal style
			case netFieldType_e::regular:
				MSG_WriteRegular(msg, field->bits, toF,demoType);
				break;
			case netFieldType_e::angle:
				MSG_WritePackedAngle(msg, *(float*)toF, field->bits, demoType);
				break;
			case netFieldType_e::animTime:
				MSG_WritePackedAnimTime(msg, *(float*)fromF, *(float*)toF, frameTime, field->bits, demoType);
				break;
			case netFieldType_e::animWeight:
				MSG_WritePackedAnimWeight(msg, *(float*)toF, field->bits, demoType);
				break;
			case netFieldType_e::scale:
				MSG_WritePackedScale(msg, *(float*)toF, field->bits, demoType);
				break;
			case netFieldType_e::alpha:
				MSG_WritePackedAlpha(msg, *(float*)toF, field->bits, demoType);
				break;
			case netFieldType_e::coord:
				MSG_WritePackedCoord(msg, *(float*)fromF, *(float*)toF, field->bits, demoType);
				break;
			case netFieldType_e::coordExtra:
				// Team Assault
				MSG_WritePackedCoordExtra(msg, *(float*)fromF, *(float*)toF, field->bits, demoType);
				break;
			case netFieldType_e::velocity:
				MSG_WritePackedVelocity(msg, *(float*)toF, field->bits);
				break;
			case netFieldType_e::simple:
				MSG_WritePackedSimple(msg, *(int*)toF, field->bits);
				break;
			default:
				Com_Error(ERR_DROP, "MSG_WriteDeltaEntity: unrecognized entity field type %i for field %i\n", field->bits, i);
				break;
			}
		}
		else {
			if (field->bits == 0) {
				// float
				fullFloat = *(float*)toF;
				trunc = (int)fullFloat;

				if (fullFloat == 0.0f) {
					MSG_WriteBits(msg, 0, 1);
					oldsize += FLOAT_INT_BITS;
				}
				else {
					MSG_WriteBits(msg, 1, 1);
					if (trunc == fullFloat && trunc + FLOAT_INT_BIAS >= 0 &&
						trunc + FLOAT_INT_BIAS < (1 << FLOAT_INT_BITS)) {
						// send as small integer
						MSG_WriteBits(msg, 0, 1);
						MSG_WriteBits(msg, trunc + FLOAT_INT_BIAS, FLOAT_INT_BITS);
					}
					else {
						// send as full floating point value
						MSG_WriteBits(msg, 1, 1);
						MSG_WriteBits(msg, *toF, 32);
					}
				}
			}
			else {
				if (*toF == 0) {
					MSG_WriteBits(msg, 0, 1);
				}
				else {
					MSG_WriteBits(msg, 1, 1);
					// integer
					MSG_WriteBits(msg, *toF, field->bits);
				}
			}
		}
	}
}

/*
==================
MSG_ReadDeltaEntity

The entity number has already been read from the message, which
is how the from state is identified.

If the delta removes the entity, entityState_t->number will be set to MAX_GENTITIES-1

Can go from either a baseline or a previous packet_entity
==================
*/
//extern	cvar_t	*cl_shownet;
#ifdef RELDEBUG
//#pragma optimize("", off)
#endif
void MSG_ReadDeltaEntity( msg_t *msg, entityState_t *from, entityState_t *to, 
						 int number,demoType_t demoType, float frameTime) {
	int			i, lc;
	int			numFields;
	const netField_t	*field;
	int			*fromF, *toF;
	int			print;
	int			trunc;
	int			startBit, endBit;
	
	bool isMOHAADemo = demoTypeIsMOHAA(demoType);

	if ( number < 0 || number >= MAX_GENTITIES) {
		Com_Error( ERR_DROP, "Bad delta entity number: %i", number );
	}

	if ( msg->bit == 0 ) {
		startBit = msg->readcount * 8 - GENTITYNUM_BITS;
	} else {
		startBit = ( msg->readcount - 1 ) * 8 + msg->bit - GENTITYNUM_BITS;
	}

	// check for a remove
	if ( MSG_ReadBits( msg, 1 ) == 1 ) {
		Com_Memset( to, 0, sizeof( *to ) );	
		to->number = MAX_GENTITIES - 1;
		/*if ( cl_shownet->integer >= 2 || cl_shownet->integer == -1 ) {
			Com_Printf( "%3i: #%-3i remove\n", msg->readcount, number );
		}*/
		return;
	}

	// check for no delta
	if ( MSG_ReadBits( msg, 1 ) == 0 ) {
		*to = *from;
		to->number = number;
		return;
	}

	const netField_t* esFields = NULL;
	/*if (demo15detected)
		numFields = sizeof(entityStateFields15)/sizeof(entityStateFields15[0]);
	else
		numFields = sizeof(entityStateFields)/sizeof(entityStateFields[0]);*/
	if (!getEntityStateFields(&esFields, &numFields, demoType)) {
		throw std::exception("could not find entitystatefields for reading demotype");
	}
	/*switch (demoType) {
	case DM_15:
		numFields = sizeof(entityStateFields15) / sizeof(entityStateFields15[0]);
		esFields = entityStateFields15;
		break;
	case DM_15_1_03:
	case DM_16:
		numFields = sizeof(entityStateFields) / sizeof(entityStateFields[0]);
		esFields = entityStateFields;
		break;
	case DM_26:
		numFields = sizeof(entityStateFieldsJKA) / sizeof(entityStateFieldsJKA[0]);
		esFields = entityStateFieldsJKA;
		break;
	}*/
	lc = MSG_ReadByte(msg);

	if (lc > numFields || lc < 0) {
#if 0
		throw malformed_message_exception(va("invalid entityState field count (got: %i, expecting: %i)", lc, numFields)); // It's more elegant but usually that demo is too broken to be saved and this won't be logged so better let it access violate. (for example overwriting other demo and such, tho we could try and find a way to save those)
#else
		Com_Error(ERR_DROP, "invalid entityState field count (got: %i, expecting: %i)", lc, numFields);
#endif
	}

	// shownet 2/3 will interleave with other printed info, -1 will
	// just print the delta records`
	/*if ( cl_shownet->integer >= 2 || cl_shownet->integer == -1 ) {
		print = 1;
		Com_Printf( "%3i: #%-3i ", msg->readcount, to->number );
	} else*/ {
		print = 0;
	}

	to->number = number;

#ifdef _DONETPROFILE_
	int startBytes,endBytes;
#endif

	//for ( i = 0, field = demo15detected ? entityStateFields15 : entityStateFields ; i < lc ; i++, field++ ) {
	for ( i = 0, field = esFields; i < lc ; i++, field++ ) {
		//if (i >= numFields) {
		//	continue; // hardening against corrupted demos. likely won't recover anyway but oh well. Actualy, strike that, didnt help anyway
		//}
		fromF = (int *)( (byte *)from + field->offset );
		toF = (int *)( (byte *)to + field->offset );

#ifdef _DONETPROFILE_
		startBytes=msg->readcount;
#endif
		

		if ( !MSG_ReadBits( msg, 1 ) ) {
			// no change
			*toF = *fromF;
		} else {

			if (isMOHAADemo) {
				// MOHAA demos are ... very special.
				switch (field->type) {
				case netFieldType_e::regular:
					MSG_ReadRegular(msg, field->bits, toF, demoType);
					break;
				case netFieldType_e::angle: // angles, what a mess! it wouldnt surprise me if something goes wrong here ;)
					*(float*)toF = MSG_ReadPackedAngle(msg, field->bits, demoType);
					break;
				case netFieldType_e::animTime: // time
					*(float*)toF = MSG_ReadPackedAnimTime(msg, field->bits, *(float*)fromF, frameTime, demoType);
					break;
				case netFieldType_e::animWeight: // nasty!
					*(float*)toF = MSG_ReadPackedAnimWeight(msg, field->bits, demoType);
					break;
				case netFieldType_e::scale:
					*(float*)toF = MSG_ReadPackedScale(msg, field->bits, demoType);
					break;
				case netFieldType_e::alpha:
					*(float*)toF = MSG_ReadPackedAlpha(msg, field->bits, demoType);
					break;
				case netFieldType_e::coord:
					*(float*)toF = MSG_ReadPackedCoord(msg, *(float*)fromF, field->bits, demoType);
					break;
				case netFieldType_e::coordExtra:
					*(float*)toF = MSG_ReadPackedCoordExtra(msg, *(float*)fromF, field->bits, demoType);
					break;
				case netFieldType_e::velocity:
					*(float*)toF = MSG_ReadPackedVelocity(msg, field->bits);
					break;
				case netFieldType_e::simple:
					*(int*)toF = MSG_ReadPackedSimple(msg, *(int*)fromF, field->bits);
					break;
				default:
					Com_Error(ERR_DROP, "MSG_ReadDeltaEntity: unrecognized entity field type %i for field\n", i);
					break;
				}

			}
			else {

				if (field->bits == 0) {
					// float
					if (MSG_ReadBits(msg, 1) == 0) {
						*(float*)toF = 0.0f;
					}
					else {
						if (MSG_ReadBits(msg, 1) == 0) {
							// integral float
							trunc = MSG_ReadBits(msg, FLOAT_INT_BITS);
							// bias to allow equal parts positive and negative
							trunc -= FLOAT_INT_BIAS;
							*(float*)toF = trunc;
							if (print) {
								Com_Printf("%s:%i ", field->name, trunc);
							}
						}
						else {
							// full floating point value
							*toF = MSG_ReadBits(msg, 32);
							if (print) {
								Com_Printf("%s:%f ", field->name, *(float*)toF);
							}
						}
					}
				}
				else {
					if (MSG_ReadBits(msg, 1) == 0) {
						*toF = 0;
					}
					else {
						// integer
						*toF = MSG_ReadBits(msg, field->bits);
						if (print) {
							Com_Printf("%s:%i ", field->name, *toF);
						}
					}
				}
				//			pcount[i]++;
			}
		}
#ifdef _DONETPROFILE_
		endBytes=msg->readcount;
		ClReadProf().AddField(field->name,endBytes-startBytes);
#endif
	}
	//for ( i = lc, field = demo15detected ? &entityStateFields15[lc] : &entityStateFields[lc] ; i < numFields ; i++, field++ ) {
	for ( i = lc, field =  &esFields[lc] ; i < numFields ; i++, field++ ) {
		if (i >= 0) { // some corrupted (?) messages have negative lc
			fromF = (int*)((byte*)from + field->offset);
			toF = (int*)((byte*)to + field->offset);
			// no change
			*toF = *fromF;
		}
	}

	if ( print ) {
		if ( msg->bit == 0 ) {
			endBit = msg->readcount * 8 - GENTITYNUM_BITS;
		} else {
			endBit = ( msg->readcount - 1 ) * 8 + msg->bit - GENTITYNUM_BITS;
		}
		Com_Printf( " (%i bits)\n", endBit - startBit  );
	}
}
#ifdef RELDEBUG
//#pragma optimize("", on)
#endif


/*
=============
MSG_WriteDeltaPlayerstate

=============
*/
void MSG_WriteDeltaPlayerstate( msg_t *msg, struct playerState_s *from, struct playerState_s *to, qboolean isVehiclePS, demoType_t demoType, float frameTime) {
	int				i;
	playerState_t	dummy;
	int				statsbits;
	int				persistantbits;
	int				ammobits;
	int				activeitemsbits;
	int				ammo_amountbits;
	int				max_ammo_amountbits;
	int				powerupbits;
	int				inventorybits;
	int				numFields;
	int				c;
	const netField_t		*playerStateFieldsHere;
	const netField_t		*field;
	int				*fromF, *toF;
	float			fullFloat;
	int				trunc, lc;

	bool isMOHAADemo = demoTypeIsMOHAA(demoType);

	if (!from) {
		from = &dummy;
		Com_Memset (&dummy, 0, sizeof(dummy));
	}

	c = msg->cursize;

	if (!getPlayerStateFields(&playerStateFieldsHere, &numFields, demoType)) {
		throw std::exception("failed to get playerstatefields for writing.");
	}
	if (playerStateRequiresSpecialHandling(demoType)) {
		switch (demoType) {
		case DM_25:
		case DM_26:
			//=====_OPTIMIZED_VEHICLE_NETWORKING=======================================================================
#ifdef _OPTIMIZED_VEHICLE_NETWORKING
			if (isVehiclePS)
			{//a vehicle playerstate
				numFields = sizeof(vehPlayerStateFieldsJKA) / sizeof(vehPlayerStateFieldsJKA[0]);
				playerStateFieldsHere = vehPlayerStateFieldsJKA;
			}
			else
			{//regular client playerstate
				if (to->m_iVehicleNum
					&& (to->eFlags & EF_NODRAW))
				{//pilot riding *inside* a vehicle!
					MSG_WriteBits(msg, 1, 1);	// Pilot player state
					numFields = sizeof(pilotPlayerStateFieldsJKA) / sizeof(pilotPlayerStateFieldsJKA[0]) - 82;
					playerStateFieldsHere = pilotPlayerStateFieldsJKA;
				}
				else
				{//normal client
					MSG_WriteBits(msg, 0, 1);	// Normal player state
					numFields = sizeof(playerStateFieldsJKA) / sizeof(playerStateFieldsJKA[0]);
					playerStateFieldsHere = playerStateFieldsJKA;
				}
			}
			//=====_OPTIMIZED_VEHICLE_NETWORKING=======================================================================

#else// _OPTIMIZED_VEHICLE_NETWORKING
			numFields = sizeof(playerStateFieldsJKA) / sizeof(playerStateFieldsJKA[0]);
			playerStateFieldsHere = playerStateFieldsJKA;
#endif// _OPTIMIZED_VEHICLE_NETWORKING
			break;
		default:
			throw std::exception("demoType doesn't have playerstate writing implemented yet.");
			break;
		}
	}
	//if (demo15detected)
	//	numFields = sizeof( playerStateFields15 ) / sizeof( playerStateFields15[0] );
	//else
	//	numFields = sizeof( playerStateFields ) / sizeof( playerStateFields[0] );

	lc = 0;
	for ( i = 0, field = playerStateFieldsHere; i < numFields ; i++, field++ ) {
		fromF = (int *)( (byte *)from + field->offset );
		toF = (int *)( (byte *)to + field->offset );
		if ( *fromF != *toF ) {
			lc = i+1;
//#ifndef FINAL_BUILD // JKA stuff, forget about it
//			field->mCount++;
//#endif
		}
	}

	MSG_WriteByte( msg, lc );	// # of changes

#ifndef FINAL_BUILD
	gLastBitIndex = lc;
#endif

	oldsize += numFields - lc;

	for ( i = 0, field = playerStateFieldsHere; i < lc ; i++, field++ ) {
		fromF = (int *)( (byte *)from + field->offset );
		toF = (int *)( (byte *)to + field->offset );

		if ( *fromF == *toF ) {
			MSG_WriteBits( msg, 0, 1 );	// no change
			continue;
		}

		MSG_WriteBits( msg, 1, 1 );	// changed
//		pcount[i]++;

		if (isMOHAADemo) {
			switch (field->type) {
			case netFieldType_e::regular:
				MSG_WriteRegularSimple(msg, field->bits, toF, demoType);
				break;
			case netFieldType_e::angle:
				MSG_WritePackedAngle(msg, *(float*)toF, field->bits, demoType);
				break;
			case netFieldType_e::animTime:
				MSG_WritePackedAnimTime(msg, *(float*)fromF, *(float*)toF, frameTime, field->bits, demoType);
				break;
			case netFieldType_e::animWeight:
				MSG_WritePackedAnimWeight(msg, *(float*)toF, field->bits, demoType);
				break;
			case netFieldType_e::scale:
				MSG_WritePackedScale(msg, *(float*)toF, field->bits, demoType);
				break;
			case netFieldType_e::alpha:
				MSG_WritePackedAlpha(msg, *(float*)toF, field->bits, demoType);
				break;
			case netFieldType_e::coord:
				MSG_WritePackedCoord(msg, *(float*)fromF, *(float*)toF, field->bits, demoType);
				break;
			case netFieldType_e::coordExtra:
				// Team Assault
				MSG_WritePackedCoordExtra(msg, *(float*)fromF, *(float*)toF, field->bits, demoType);
				break;
			case netFieldType_e::velocity:
				MSG_WritePackedVelocity(msg, *(float*)toF, field->bits);
				break;
			case netFieldType_e::simple:
				MSG_WritePackedSimple(msg, *(int*)toF, field->bits);
				break;
			default:
				break;
			}
		}
		else {
			if ( field->bits == 0 ) {
				// float
				fullFloat = *(float *)toF;
				trunc = (int)fullFloat;

				if ( trunc == fullFloat && trunc + FLOAT_INT_BIAS >= 0 && 
					trunc + FLOAT_INT_BIAS < ( 1 << FLOAT_INT_BITS ) ) {
					// send as small integer
					MSG_WriteBits( msg, 0, 1 );
					MSG_WriteBits( msg, trunc + FLOAT_INT_BIAS, FLOAT_INT_BITS );
				} else {
					// send as full floating point value
					MSG_WriteBits( msg, 1, 1 );
					MSG_WriteBits( msg, *toF, 32 );
				}
			} else {
				// integer
				MSG_WriteBits( msg, *toF, field->bits );
			}
		}
	}
	c = msg->cursize - c;


	//
	// send the arrays
	//
	statsbits = 0;
	int statCount = isMOHAADemo ? 32 : 16;
	for (i=0 ; i< statCount; i++) {
		if (to->stats[i] != from->stats[i]) {
			statsbits |= 1<<i;
		}
	}
	if (isMOHAADemo) {
		activeitemsbits = 0;
		for (i = 0; i < MAX_ACTIVEITEMS; i++) {
			if (to->activeItems[i] != from->activeItems[i]) {
				activeitemsbits |= 1 << i;
			}
		}
		ammo_amountbits = 0;
		for (i = 0; i < MAX_AMMO_AMOUNT; i++) {
			if (to->ammo_amount[i] != from->ammo_amount[i]) {
				ammo_amountbits |= 1 << i;
			}
		}
		ammobits = 0;
		for (i = 0; i < MAX_WEAPONS; i++) {
			if (to->ammo_name_index[i] != from->ammo_name_index[i]) {
				ammobits |= 1 << i;
			}
		}
		max_ammo_amountbits = 0;
		for (i = 0; i < MAX_MAX_AMMO_AMOUNT; i++) {
			if (to->max_ammo_amount[i] != from->max_ammo_amount[i]) {
				max_ammo_amountbits |= 1 << i;
			}
		}
		if (!statsbits && !activeitemsbits && !ammobits && !ammo_amountbits && !max_ammo_amountbits) {
			MSG_WriteBits(msg, 0, 1);	// no change
			oldsize += 5;
			return;
		}
	}
	else {
		persistantbits = 0;
		for (i = 0; i < 16; i++) {
			if (to->persistant[i] != from->persistant[i]) {
				persistantbits |= 1 << i;
			}
		}
		ammobits = 0;
		for (i = 0; i < 16; i++) {
			if (to->ammo[i] != from->ammo[i]) {
				ammobits |= 1 << i;
			}
		}
		powerupbits = 0;
		for (i = 0; i < 16; i++) {
			if (to->powerups[i] != from->powerups[i]) {
				powerupbits |= 1 << i;
			}
		}

		if (demoType == DM_14) {
			inventorybits = 0;
			for (i = 0; i < MAX_INVENTORY; i++) {
				if (to->inventory[i] != from->inventory[i]) {
					inventorybits |= 1 << i;
				}
			}
		}
		if (!statsbits && !persistantbits && !ammobits && !powerupbits) {
			MSG_WriteBits(msg, 0, 1);	// no change
			oldsize += 4;
			return;
		}
	}

	
	MSG_WriteBits( msg, 1, 1 );	// changed

	if ( statsbits ) {
		MSG_WriteBits( msg, 1, 1 );	// changed
		if (isMOHAADemo) {
			MSG_WriteLong(msg, statsbits);
		}
		else {
			MSG_WriteShort(msg, statsbits);
		}
		for (i = 0; i < statCount; i++) {

			if (statsbits & (1 << i)) {
				if (demoType == DM_26 || demoType == DM_25) {
					if (i == STAT_WEAPONS)
					{ //ugly.. but we're gonna need it anyway -rww
						//(just send this one in MAX_WEAPONS bits, so that we can add up to MAX_WEAPONS weaps without hassle)
						MSG_WriteBits(msg, to->stats[i], MAX_WEAPONS_JKA);
					}
					else
					{
						MSG_WriteShort(msg, to->stats[i]);
					}
				}
				else if (demoType == DM_14) {
					MSG_WriteSShort(msg, to->stats[i]);
				}
				else {
					MSG_WriteShort(msg, to->stats[i]);
				}
			}
		}
	} else {
		MSG_WriteBits( msg, 0, 1 );	// no change
	}

	if (isMOHAADemo) {

		if (activeitemsbits) {
			MSG_WriteBits(msg, 1, 1);	// changed
			MSG_WriteBits(msg, activeitemsbits, MAX_ACTIVEITEMS);
			for (i = 0; i < MAX_ACTIVEITEMS; i++)
				if (activeitemsbits & (1 << i))
					MSG_WriteShort(msg, to->activeItems[i]);
		}
		else {
			MSG_WriteBits(msg, 0, 1);	// no change
		}

		if (ammo_amountbits) {
			MSG_WriteBits(msg, 1, 1);	// changed
			MSG_WriteBits(msg, ammo_amountbits, MAX_AMMO_AMOUNT);
			for (i = 0; i < MAX_AMMO_AMOUNT; i++)
				if (ammo_amountbits & (1 << i))
					MSG_WriteShort(msg, to->ammo_amount[i]);
		}
		else {
			MSG_WriteBits(msg, 0, 1);	// no change
		}


		if (ammobits) {
			MSG_WriteBits(msg, 1, 1);	// changed
			MSG_WriteBits(msg, ammobits, MAX_AMMO);
			for (i = 0; i < MAX_AMMO; i++)
				if (ammobits & (1 << i))
					MSG_WriteShort(msg, to->ammo_name_index[i]);
		}
		else {
			MSG_WriteBits(msg, 0, 1);	// no change
		}


		if (max_ammo_amountbits) {
			MSG_WriteBits(msg, 1, 1);	// changed
			MSG_WriteBits(msg, max_ammo_amountbits, MAX_MAX_AMMO_AMOUNT);
			for (i = 0; i < MAX_MAX_AMMO_AMOUNT; i++)
				if (max_ammo_amountbits & (1 << i))
					MSG_WriteShort(msg, to->max_ammo_amount[i]);
		}
		else {
			MSG_WriteBits(msg, 0, 1);	// no change
		}
	}
	else {

		if (persistantbits) {
			MSG_WriteBits(msg, 1, 1);	// changed
			MSG_WriteShort(msg, persistantbits);
			for (i = 0; i < 16; i++) {
				if (persistantbits & (1 << i)) {
					if (demoType == DM_14) {
						MSG_WriteSShort(msg, to->persistant[i]);
					}
					else {
						MSG_WriteShort(msg, to->persistant[i]);
					}
				}
			}
		}
		else {
			MSG_WriteBits(msg, 0, 1);	// no change
		}


		if (ammobits) {
			MSG_WriteBits(msg, 1, 1);	// changed
			MSG_WriteShort(msg, ammobits);
			for (i = 0; i < 16; i++) {
				if (ammobits & (1 << i)) {
					if (demoType == DM_14) {
						MSG_WriteSShort(msg, to->ammo[i]);
					}
					else {
						MSG_WriteShort(msg, to->ammo[i]);
					}
				}
			}
		}
		else {
			MSG_WriteBits(msg, 0, 1);	// no change
		}


		if (powerupbits) {
			MSG_WriteBits(msg, 1, 1);	// changed
			MSG_WriteShort(msg, powerupbits);
			for (i = 0; i < 16; i++)
				if (powerupbits & (1 << i))
					MSG_WriteLong(msg, to->powerups[i]);
		}
		else {
			MSG_WriteBits(msg, 0, 1);	// no change
		}

		if (demoType == DM_14) {
			if (inventorybits) {
				MSG_WriteBits(msg, 1, 1);	// changed
				MSG_WriteShort(msg, inventorybits);
				for (i = 0; i < 16; i++)
					if (inventorybits & (1 << i))
						MSG_WriteShort(msg, to->inventory[i]);
			}
			else {
				MSG_WriteBits(msg, 0, 1);	// no change
			}
		}
	}
	
}


/*
===================
MSG_ReadDeltaPlayerstate
===================
*/
//void MSG_ReadDeltaPlayerstate (msg_t *msg, playerState_t *from, playerState_t *to, qboolean demo15detected ) {
void MSG_ReadDeltaPlayerstate (msg_t *msg, playerState_t *from, playerState_t *to, demoType_t demoType, qboolean isVehiclePS) { // vehicle ps stuff is for JKA
	int			i, lc;
	int			bits;
	const netField_t	*field;
	int			numFields;
	int			startBit, endBit;
	int			print;
	int			*fromF, *toF;
	int			trunc;
	playerState_t	dummy;

	bool isMOHAADemo = demoTypeIsMOHAA(demoType);

	if ( !from ) {
		from = &dummy;
		Com_Memset( &dummy, 0, sizeof( dummy ) );
	}
	*to = *from;

	if ( msg->bit == 0 ) {
		startBit = msg->readcount * 8 - GENTITYNUM_BITS;
	} else {
		startBit = ( msg->readcount - 1 ) * 8 + msg->bit - GENTITYNUM_BITS;
	}

	// shownet 2/3 will interleave with other printed info, -2 will
	// just print the delta records
	/*if ( cl_shownet->integer >= 2 || cl_shownet->integer == -2 ) {
		print = 1;
		Com_Printf( "%3i: playerstate ", msg->readcount );
	} else*/ {
		print = 0;
	}

	const netField_t* psFields = NULL;
	/*if (demo15detected)
		numFields = sizeof( playerStateFields15 ) / sizeof( playerStateFields15[0] );
	else
		numFields = sizeof( playerStateFields ) / sizeof( playerStateFields[0] );*/

	if (!getPlayerStateFields(&psFields,&numFields,demoType)) {
		throw std::exception("could not get playerstatefields for reading demotype");
	}
	if (playerStateRequiresSpecialHandling(demoType)) {
		switch (demoType) {
			/*case DM_15:
				numFields = sizeof(playerStateFields15) / sizeof(playerStateFields15[0]);
				psFields = playerStateFields15;
				break;
			case DM_15_1_03:
			case DM_16:
				numFields = sizeof(playerStateFields) / sizeof(playerStateFields[0]);
				psFields = playerStateFields;
				break;*/
			case DM_25:
			case DM_26:
				psFields = playerStateFieldsJKA;
				//=====_OPTIMIZED_VEHICLE_NETWORKING=======================================================================
#ifdef _OPTIMIZED_VEHICLE_NETWORKING
				if (isVehiclePS)
				{//a vehicle playerstate
					numFields = sizeof(vehPlayerStateFieldsJKA) / sizeof(vehPlayerStateFieldsJKA[0]);
					psFields = vehPlayerStateFieldsJKA;
				}
				else
				{
					int isPilot = MSG_ReadBits(msg, 1);
					if (isPilot)
					{//pilot riding *inside* a vehicle!
						numFields = sizeof(pilotPlayerStateFieldsJKA) / sizeof(pilotPlayerStateFieldsJKA[0]) -82; // Why -82? jedis repository has this but eternaljk doesnt. Hmm.
						psFields = pilotPlayerStateFieldsJKA;
					}
					else
					{//normal client
						numFields = sizeof(playerStateFieldsJKA) / sizeof(playerStateFieldsJKA[0]);
					}
				}
				//=====_OPTIMIZED_VEHICLE_NETWORKING=======================================================================
#else//_OPTIMIZED_VEHICLE_NETWORKING
				numFields = sizeof(playerStateFieldsJKA) / sizeof(playerStateFieldsJKA[0]);
#endif//_OPTIMIZED_VEHICLE_NETWORKING
				break;
			default:
				throw std::exception("handling of playerstate reading for this demoType not supported");
				break;
		}
	}
	
	lc = MSG_ReadByte(msg);

	if (lc > numFields || lc < 0)
		Com_Error(ERR_DROP, "invalid playerState field count (got: %i, expecting: %i)", lc, numFields);

#ifdef _DONETPROFILE_
	int startBytes,endBytes;
#endif

	//for ( i = 0, field = demo15detected ? playerStateFields15 : playerStateFields ; i < lc ; i++, field++ ) {
	for ( i = 0, field = psFields ; i < lc ; i++, field++ ) {
		fromF = (int *)( (byte *)from + field->offset );
		toF = (int *)( (byte *)to + field->offset );

#ifdef _DONETPROFILE_
		startBytes=msg->readcount;
#endif
		if ( ! MSG_ReadBits( msg, 1 ) ) {
			// no change
			*toF = *fromF;
		} else {

			if (isMOHAADemo) {

				switch (field->type) {
				case netFieldType_e::regular:
					MSG_ReadRegularSimple(msg, field->bits, toF,demoType);
					break;
				case netFieldType_e::angle:
					*(float*)toF = MSG_ReadPackedAngle(msg, field->bits, demoType);
					break;
				case netFieldType_e::coord:
					*(float*)toF = MSG_ReadPackedCoord(msg, *(float*)fromF, field->bits, demoType);
					break;
				case netFieldType_e::coordExtra:
					*(float*)toF = MSG_ReadPackedCoordExtra(msg, *(float*)fromF, field->bits, demoType);
					break;
				case netFieldType_e::velocity:
					*(float*)toF = MSG_ReadPackedVelocity(msg, field->bits);
					break;
				default:
					break;
				}
			}
			else {

				if ( field->bits == 0 ) {
					// float
					if ( MSG_ReadBits( msg, 1 ) == 0 ) {
						// integral float
						trunc = MSG_ReadBits( msg, FLOAT_INT_BITS );
						// bias to allow equal parts positive and negative
						trunc -= FLOAT_INT_BIAS;
						*(float *)toF = trunc; 
						if ( print ) {
							Com_Printf( "%s:%i ", field->name, trunc );
						}
					} else {
						// full floating point value
						*toF = MSG_ReadBits( msg, 32 );
						if ( print ) {
							Com_Printf( "%s:%f ", field->name, *(float *)toF );
						}
					}
				} else {
					// integer
					*toF = MSG_ReadBits( msg, field->bits );
					if ( print ) {
						Com_Printf( "%s:%i ", field->name, *toF );
					}
				}
			}
		}
#ifdef _DONETPROFILE_
		endBytes=msg->readcount;
		ClReadProf().AddField(field->name,endBytes-startBytes);
#endif
	}
	//for ( i=lc,field = demo15detected ? &playerStateFields15[lc] : &playerStateFields[lc];i<numFields; i++, field++) {
	for ( i=lc,field = &psFields[lc];i<numFields; i++, field++) {
		fromF = (int *)( (byte *)from + field->offset );
		toF = (int *)( (byte *)to + field->offset );
		// no change
		*toF = *fromF;
	}

	// read the arrays
#ifdef _DONETPROFILE_
		startBytes=msg->readcount;
#endif
	if (MSG_ReadBits( msg, 1 ) ) {
		// parse stats
		if ( MSG_ReadBits( msg, 1 ) ) {
			//LOG("PS_STATS");

			bits = isMOHAADemo ? MSG_ReadLong(msg) : MSG_ReadShort(msg);
			int statCount = isMOHAADemo ? 32 : 16;
			for (i=0 ; i< statCount; i++) {
				if (bits & (1<<i) ) {
					if ((demoType == DM_26 || demoType == DM_25)&& i == STAT_WEAPONS)
					{ //ugly.. but we're gonna need it anyway -rww
						to->stats[i] = MSG_ReadBits(msg, MAX_WEAPONS_JKA);
					}
					else if(demoType == DM_14)
					{
						to->stats[i] = MSG_ReadSShort(msg); // Signed for some reason. Idk if really needed
					}
					else
					{
						to->stats[i] = MSG_ReadShort(msg);
					}
				}
			}
		}
#ifdef _DONETPROFILE_
		endBytes=msg->readcount;
		ClReadProf().AddField("PS_STATS",endBytes-startBytes);
#endif

		if (isMOHAADemo) {

			// parse activeItems
			if (MSG_ReadBits(msg, 1)) {
				//LOG("PS_ITEMS");
				bits = MSG_ReadByte(msg);
				for (i = 0; i < MAX_ACTIVEITEMS; i++) {
					if (bits & (1 << i)) {
						to->activeItems[i] = MSG_ReadShort(msg);
			}
		}
	}

			// parse ammo_amount
			if (MSG_ReadBits(msg, 1)) {
				//LOG("PS_AMMO_AMOUNT");
				bits = MSG_ReadShort(msg);
				for (i = 0; i < MAX_AMMO_AMOUNT; i++) {
					if (bits & (1 << i)) {
						to->ammo_amount[i] = MSG_ReadShort(msg);
					}
				}
			}

			// parse ammo_name_index
			if (MSG_ReadBits(msg, 1)) {
				//LOG("PS_AMMO");
				bits = MSG_ReadShort(msg);
				for (i = 0; i < MAX_AMMO; i++) {
					if (bits & (1 << i)) {
						to->ammo_name_index[i] = MSG_ReadShort(msg);
					}
				}
			}

			// parse powerups
			if (MSG_ReadBits(msg, 1)) {
				//LOG("PS_MAX_AMMO_AMOUNT");
				bits = MSG_ReadShort(msg);
				for (i = 0; i < MAX_MAX_AMMO_AMOUNT; i++) {
					if (bits & (1 << i)) {
						to->max_ammo_amount[i] = MSG_ReadShort(msg);
					}
				}
			}
		}
		else {

			// parse persistant stats
	#ifdef _DONETPROFILE_
			startBytes=msg->readcount;
	#endif
			if ( MSG_ReadBits( msg, 1 ) ) {
				//LOG("PS_PERSISTANT");
				bits = MSG_ReadShort (msg);
				for (i=0 ; i<16 ; i++) {
					if (bits & (1<<i) ) {
						if (demoType == DM_14) {
							to->persistant[i] = MSG_ReadSShort(msg); // Signed for some reason. Idk if really needed
						}
						else {
							to->persistant[i] = MSG_ReadShort(msg);
						}
					}
				}
			}
	#ifdef _DONETPROFILE_
			endBytes=msg->readcount;
			ClReadProf().AddField("PS_PERSISTANT",endBytes-startBytes);
	#endif

			// parse ammo
	#ifdef _DONETPROFILE_
			startBytes=msg->readcount;
	#endif
			if ( MSG_ReadBits( msg, 1 ) ) {
				//LOG("PS_AMMO");
				bits = MSG_ReadShort (msg);
				for (i=0 ; i<16 ; i++) {
					if (bits & (1<<i) ) {
						if (demoType == DM_14) {
							to->ammo[i] = MSG_ReadSShort(msg);
						}
						else {
							to->ammo[i] = MSG_ReadShort(msg);
						}
					}
				}
			}
	#ifdef _DONETPROFILE_
			endBytes=msg->readcount;
			ClReadProf().AddField("PS_AMMO",endBytes-startBytes);
	#endif

			// parse powerups
	#ifdef _DONETPROFILE_
			startBytes=msg->readcount;
	#endif
			if ( MSG_ReadBits( msg, 1 ) ) {
				//LOG("PS_POWERUPS");
				bits = MSG_ReadShort (msg);
				for (i=0 ; i<16 ; i++) {
					if (bits & (1<<i) ) {
						to->powerups[i] = MSG_ReadLong(msg);
					}
				}
			}
		
			if (demoType == DM_14) {

				// parse inventory (jk2 sp only)
	#ifdef _DONETPROFILE_
				startBytes = msg->readcount;
	#endif
				if (MSG_ReadBits(msg, 1)) {
					//LOG("PS_INVENTORY");
					bits = MSG_ReadShort(msg);
					for (i = 0; i < 16; i++) { // Should only go to 15 technically (MAX_INVENTORY) but it's not like the bits are ever gonna contain more anyway...
						if (bits & (1 << i)) {
							to->inventory[i] = MSG_ReadShort(msg);
						}
					}
				}
			}

		}
	}
#ifdef _DONETPROFILE_
		endBytes=msg->readcount;
		ClReadProf().AddField("PS_POWERUPS",endBytes-startBytes);
#endif

	if ( print ) {
		if ( msg->bit == 0 ) {
			endBit = msg->readcount * 8 - GENTITYNUM_BITS;
		} else {
			endBit = ( msg->readcount - 1 ) * 8 + msg->bit - GENTITYNUM_BITS;
		}
		Com_Printf( " (%i bits)\n", endBit - startBit  );
	}
}

/*
// New data gathered to tune Q3 to JK2MP. Takes longer to crunch and gain was minimal.
int msg_hData[256] = 
{
	3163878,		// 0
	473992,			// 1
	564019,			// 2
	136497,			// 3
	129559,			// 4
	283019,			// 5
	75812,			// 6
	179836,			// 7
	85958,			// 8
	168542,			// 9
	78898,			// 10
	82007,			// 11
	48613,			// 12
	138741,			// 13
	35482,			// 14
	47433,			// 15
	65214,			// 16
	51636,			// 17
	63741,			// 18
	52823,			// 19
	42464,			// 20
	44495,			// 21
	45347,			// 22
	40260,			// 23
	59168,			// 24
	44990,			// 25
	52957,			// 26
	42700,			// 27
	42414,			// 28
	36451,			// 29
	45653,			// 30
	44667,			// 31
	125336,			// 32
	38435,			// 33
	53658,			// 34
	42621,			// 35
	40932,			// 36
	33409,			// 37
	35470,			// 38
	40769,			// 39
	33813,			// 40
	32480,			// 41
	33664,			// 42
	32303,			// 43
	32394,			// 44
	34822,			// 45
	37724,			// 46
	48016,			// 47
	94212,			// 48
	53774,			// 49
	54522,			// 50
	44044,			// 51
	42800,			// 52
	47597,			// 53
	29742,			// 54
	30237,			// 55
	34291,			// 56
	106496,			// 57
	20963,			// 58
	19342,			// 59
	20603,			// 60
	19568,			// 61
	23013,			// 62
	23939,			// 63
	44995,			// 64
	37128,			// 65
	44264,			// 66
	46636,			// 67
	56400,			// 68
	32746,			// 69
	23458,			// 70
	29702,			// 71
	25305,			// 72
	20159,			// 73
	19645,			// 74
	20593,			// 75
	21729,			// 76
	19362,			// 77
	24760,			// 78
	22788,			// 79
	25085,			// 80
	21074,			// 81
	97271,			// 82
	22048,			// 83
	24131,			// 84
	19287,			// 85
	20296,			// 86
	20131,			// 87
	86477,			// 88
	25352,			// 89
	20872,			// 90
	21382,			// 91
	38744,			// 92
	137256,			// 93
	26025,			// 94
	22243,			// 95
	23974,			// 96
	43305,			// 97
	28191,			// 98
	34638,			// 99
	37613,			// 100
	46003,			// 101
	31415,			// 102
	25746,			// 103
	28338,			// 104
	34689,			// 105
	24948,			// 106
	27110,			// 107
	39950,			// 108
	32793,			// 109
	42639,			// 110
	47883,			// 111
	37439,			// 112
	23875,			// 113
	36092,			// 114
	46471,			// 115
	37392,			// 116
	33063,			// 117
	29604,			// 118
	42140,			// 119
	61745,			// 120
	45618,			// 121
	51779,			// 122
	49684,			// 123
	57644,			// 124
	65021,			// 125
	67318,			// 126
	88197,			// 127
	258378,			// 128
	76806,			// 129
	72430,			// 130
	64936,			// 131
	62196,			// 132
	56461,			// 133
	166474,			// 134
	70036,			// 135
	40735,			// 136
	29598,			// 137
	26966,			// 138
	26093,			// 139
	25853,			// 140
	26065,			// 141
	26176,			// 142
	26777,			// 143
	26684,			// 144
	23880,			// 145
	22932,			// 146
	24566,			// 147
	24305,			// 148
	26399,			// 149
	23487,			// 150
	24485,			// 151
	25956,			// 152
	26065,			// 153
	26151,			// 154
	23111,			// 155
	23900,			// 156
	22128,			// 157
	24096,			// 158
	20863,			// 159
	24298,			// 160
	22572,			// 161
	22364,			// 162
	20813,			// 163
	21414,			// 164
	21570,			// 165
	20799,			// 166
	20971,			// 167
	22485,			// 168
	20397,			// 169
	88096,			// 170
	17802,			// 171
	20091,			// 172
	84250,			// 173
	21953,			// 174
	21406,			// 175
	23401,			// 176
	19546,			// 177
	19180,			// 178
	18843,			// 179
	20673,			// 180
	19918,			// 181
	20640,			// 182
	20326,			// 183
	21174,			// 184
	21736,			// 185
	22511,			// 186
	20290,			// 187
	23303,			// 188
	19800,			// 189
	25465,			// 190
	22801,			// 191
	28831,			// 192
	26663,			// 193
	36485,			// 194
	45768,			// 195
	49795,			// 196
	36026,			// 197
	24119,			// 198
	18543,			// 199
	19261,			// 200
	17137,			// 201
	19435,			// 202
	23672,			// 203
	22988,			// 204
	18107,			// 205
	18734,			// 206
	19847,			// 207
	101897,			// 208
	18405,			// 209
	21260,			// 210
	17818,			// 211
	18971,			// 212
	19317,			// 213
	19112,			// 214
	19395,			// 215
	20688,			// 216
	18438,			// 217
	18945,			// 218
	29309,			// 219
	19666,			// 220
	18735,			// 221
	87691,			// 222
	18478,			// 223
	22634,			// 224
	20984,			// 225
	20079,			// 226
	18624,			// 227
	20045,			// 228
	18369,			// 229
	19014,			// 230
	83179,			// 231
	20899,			// 232
	17854,			// 233
	19332,			// 234
	17875,			// 235
	28647,			// 236
	17465,			// 237
	20277,			// 238
	18994,			// 239
	22192,			// 240
	17443,			// 241
	20243,			// 242
	28174,			// 243
	134871,			// 244
	17753,			// 245
	18924,			// 246
	18281,			// 247
	18937,			// 248
	17419,			// 249
	20679,			// 250
	17865,			// 251
	17984,			// 252
	58615,			// 253
	35506,			// 254
	123499,			// 255
};
*/

// Q3 TA freq. table.
int msg_hData[256] = {
250315,			// 0
41193,			// 1
6292,			// 2
7106,			// 3
3730,			// 4
3750,			// 5
6110,			// 6
23283,			// 7
33317,			// 8
6950,			// 9
7838,			// 10
9714,			// 11
9257,			// 12
17259,			// 13
3949,			// 14
1778,			// 15
8288,			// 16
1604,			// 17
1590,			// 18
1663,			// 19
1100,			// 20
1213,			// 21
1238,			// 22
1134,			// 23
1749,			// 24
1059,			// 25
1246,			// 26
1149,			// 27
1273,			// 28
4486,			// 29
2805,			// 30
3472,			// 31
21819,			// 32
1159,			// 33
1670,			// 34
1066,			// 35
1043,			// 36
1012,			// 37
1053,			// 38
1070,			// 39
1726,			// 40
888,			// 41
1180,			// 42
850,			// 43
960,			// 44
780,			// 45
1752,			// 46
3296,			// 47
10630,			// 48
4514,			// 49
5881,			// 50
2685,			// 51
4650,			// 52
3837,			// 53
2093,			// 54
1867,			// 55
2584,			// 56
1949,			// 57
1972,			// 58
940,			// 59
1134,			// 60
1788,			// 61
1670,			// 62
1206,			// 63
5719,			// 64
6128,			// 65
7222,			// 66
6654,			// 67
3710,			// 68
3795,			// 69
1492,			// 70
1524,			// 71
2215,			// 72
1140,			// 73
1355,			// 74
971,			// 75
2180,			// 76
1248,			// 77
1328,			// 78
1195,			// 79
1770,			// 80
1078,			// 81
1264,			// 82
1266,			// 83
1168,			// 84
965,			// 85
1155,			// 86
1186,			// 87
1347,			// 88
1228,			// 89
1529,			// 90
1600,			// 91
2617,			// 92
2048,			// 93
2546,			// 94
3275,			// 95
2410,			// 96
3585,			// 97
2504,			// 98
2800,			// 99
2675,			// 100
6146,			// 101
3663,			// 102
2840,			// 103
14253,			// 104
3164,			// 105
2221,			// 106
1687,			// 107
3208,			// 108
2739,			// 109
3512,			// 110
4796,			// 111
4091,			// 112
3515,			// 113
5288,			// 114
4016,			// 115
7937,			// 116
6031,			// 117
5360,			// 118
3924,			// 119
4892,			// 120
3743,			// 121
4566,			// 122
4807,			// 123
5852,			// 124
6400,			// 125
6225,			// 126
8291,			// 127
23243,			// 128
7838,			// 129
7073,			// 130
8935,			// 131
5437,			// 132
4483,			// 133
3641,			// 134
5256,			// 135
5312,			// 136
5328,			// 137
5370,			// 138
3492,			// 139
2458,			// 140
1694,			// 141
1821,			// 142
2121,			// 143
1916,			// 144
1149,			// 145
1516,			// 146
1367,			// 147
1236,			// 148
1029,			// 149
1258,			// 150
1104,			// 151
1245,			// 152
1006,			// 153
1149,			// 154
1025,			// 155
1241,			// 156
952,			// 157
1287,			// 158
997,			// 159
1713,			// 160
1009,			// 161
1187,			// 162
879,			// 163
1099,			// 164
929,			// 165
1078,			// 166
951,			// 167
1656,			// 168
930,			// 169
1153,			// 170
1030,			// 171
1262,			// 172
1062,			// 173
1214,			// 174
1060,			// 175
1621,			// 176
930,			// 177
1106,			// 178
912,			// 179
1034,			// 180
892,			// 181
1158,			// 182
990,			// 183
1175,			// 184
850,			// 185
1121,			// 186
903,			// 187
1087,			// 188
920,			// 189
1144,			// 190
1056,			// 191
3462,			// 192
2240,			// 193
4397,			// 194
12136,			// 195
7758,			// 196
1345,			// 197
1307,			// 198
3278,			// 199
1950,			// 200
886,			// 201
1023,			// 202
1112,			// 203
1077,			// 204
1042,			// 205
1061,			// 206
1071,			// 207
1484,			// 208
1001,			// 209
1096,			// 210
915,			// 211
1052,			// 212
995,			// 213
1070,			// 214
876,			// 215
1111,			// 216
851,			// 217
1059,			// 218
805,			// 219
1112,			// 220
923,			// 221
1103,			// 222
817,			// 223
1899,			// 224
1872,			// 225
976,			// 226
841,			// 227
1127,			// 228
956,			// 229
1159,			// 230
950,			// 231
7791,			// 232
954,			// 233
1289,			// 234
933,			// 235
1127,			// 236
3207,			// 237
1020,			// 238
927,			// 239
1355,			// 240
768,			// 241
1040,			// 242
745,			// 243
952,			// 244
805,			// 245
1073,			// 246
740,			// 247
1013,			// 248
805,			// 249
1008,			// 250
796,			// 251
996,			// 252
1057,			// 253
11457,			// 254
13504,			// 255
};

#ifndef _USINGNEWHUFFTABLE_

void MSG_initHuffman() {
	int i,j;

#ifdef _NEWHUFFTABLE_
	fp=fopen("c:\\netchan.bin", "a");
#endif // _NEWHUFFTABLE_
	
	msgInit = qtrue;
#ifndef FASTHUFFMAN // For an actual client we would have to do this smarter. But this will do for the demo tools.
	Huff_Init(&msgHuff);
	for(i=0;i<256;i++) {
		for (j=0;j<msg_hData[i];j++) {
			Huff_addRef(&msgHuff.compressor,	(byte)i);			// Do update
			Huff_addRef(&msgHuff.decompressor,	(byte)i);			// Do update
		}
	}
#endif
}

#else

void MSG_initHuffman() {

	byte	*data;
	int		size, i, ch;
	int		array[256];

	msgInit = qtrue;

	Huff_Init(&msgHuff);
	// load it in
	size = FS_ReadFile( "netchan\\netchan.bin", (void **)&data );

	for(i=0;i<256;i++) {
		array[i] = 0;
	}
	for(i=0;i<size;i++) {
		ch = data[i];
		Huff_addRef(&msgHuff.compressor,	ch);			// Do update
		Huff_addRef(&msgHuff.decompressor,	ch);			// Do update
		array[ch]++;
	}
	Com_Printf("msg_hData {\n");
	for(i=0;i<256;i++) {
		if (array[i] == 0) {
			Huff_addRef(&msgHuff.compressor,	i);			// Do update
			Huff_addRef(&msgHuff.decompressor,	i);			// Do update
		}
		Com_Printf("%d,			// %d\n", array[i], i);
	}
	Com_Printf("};\n");
	FS_FreeFile( data );
	Cbuf_AddText( "condump dump.txt\n" );
}

#endif // _USINGNEWHUFFTABLE_

void MSG_shutdownHuffman()
{
#ifdef _NEWHUFFTABLE_
	if(fp)
	{
		fclose(fp);
	}
#endif // _NEWHUFFTABLE_
}

//===========================================================================
