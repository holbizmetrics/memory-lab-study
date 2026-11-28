# Extended low-pass bandwidth

On **FILTER → MODERN**, enable **MODERN ON**, then check **EXTENDED LP BW**
to let **BANDWIDTH** shape the low-pass response on the selected part.
At **1 oct** the low-pass response is unchanged. Below 1 oct the knee can
become more pronounced. Above 1 oct the knee becomes more damped.
The amount of change depends on **RESONANCE**.

The switch scales the first low-pass section's Q by the inverse square root
of bandwidth, within the existing Q limits. LP2, LP3, LP4 and any blend that
contains them use that section. Pure BP4 bandwidth, the low-pass pole count,
the cutoff setting and the Classic families keep their existing behavior.
The new switch smooths changes using the existing filter smoothing.

For an A/B, start with **Clean Foundation**. Set **MODERN ON**, **MODERN
PROFILE** to **0.500**, **BASE CUTOFF** to **900 Hz**, and **RESONANCE** to
**1.50**. With **EXTENDED LP BW** on, hold a low note and compare
**BANDWIDTH** at **0.25** and **4.00 oct**. At either extreme, switch
**EXTENDED LP BW** off to hear the Legacy response and see the bandwidth
control dim. Try **0.750** and **1.000** for LP3 and LP4.

The toggle defaults to off in older patches, older projects and starter
sounds. Patch schema 27 and host state 29 store it per part. Copy Part
includes it. The curve is a Memory Lab design based on the reported type
of behavior. The hardware's bandwidth law remains unknown.
