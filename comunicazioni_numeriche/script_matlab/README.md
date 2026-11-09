# MATLAB Scripts: Usage Guide

This guide covers the `.m` files in `~/UniPi/comunicazioni_numeriche/script_matlab` and the audio file stored beside them.

## Start MATLAB in the scripts folder

In MATLAB's Command Window, run:

```matlab
folder = fullfile(getenv("HOME"), "UniPi", "comunicazioni_numeriche", "script_matlab");
cd(folder)
addpath(folder)
```

Then run an exercise by typing its filename without `.m`, for example:

```matlab
exercise_rect
```

You can also open the folder in MATLAB's **Current Folder** panel, double-click a script, and click **Run**.

## Exercise scripts

| Script | What it does | Notes |
|---|---|---|
| `exercise_rect.m` | Creates a rectangular pulse, compares its numerical and analytic Fourier transforms, plots phase in radians, and approximates the inverse transform. | Uses `function_Fourier_Transform` and `function_Inverse_Fourier_Transform`. |
| `exercise_tri.m` | Creates a triangular pulse and compares its numerical and analytic Fourier transforms. | The inverse-transform section remains commented out. |
| `exercise_sinc.m` | Creates a sinc signal and plots its FFT-based spectrum. | Uses the local `sinc_normalized` helper and a practical target frequency resolution. |
| `exercise_sum.m` | Adds three modulated signals, plots their spectrum, then multiplies the sum by a cosine at 20 Hz and plots the result. | The multiplication is a mixing/demodulation step; it shifts spectral components. |
| `exercise_electrocardiogram.m` | Loads an ECG signal, removes its mean, and plots the signal and Fourier-transform magnitude. | Uses `100m (1).mat` beside the script if present; otherwise it asks you to select a MAT-file containing `val`. |
| `exercise_wave.m` | Reads and plots the supplied audio, computes its FFT, then generates and plays white noise. | The included audio file is stereo. Music playback is commented out; the active `sound` call plays generated noise. |

Most scripts begin with `clear` and `close all`, so running one clears workspace variables and closes existing figures.

## Helper functions

These functions are called by the exercises; normally, you do not run them directly.

| Function | Call form | Purpose |
|---|---|---|
| `function_Fourier_Transform.m` | `[G, f] = function_Fourier_Transform(g, time_samples, Ts, deltaf, durF)` | Directly approximates the continuous-time Fourier transform on frequencies from `-durF` to `durF`, processing frequencies in blocks to limit temporary memory use. |
| `function_Inverse_Fourier_Transform.m` | `[g, time_samples] = function_Inverse_Fourier_Transform(G, f, deltaf, Ts, durT)` | Approximates the inverse transform over the requested time interval. |
| `function_FFT.m` | `[G_FFT, f] = function_FFT(g, Ts, deltaf)` | Computes a centered, zero-padded FFT for samples centered around time zero, scales it to approximate a Fourier transform, and returns a matching frequency axis. |
| `function_IFFT.m` | `[g_IFFT, time_samples] = function_IFFT(G_FFT, Ts, deltaf)` | Inverts the centered spectrum returned by `function_FFT`. |
| `sinc_normalized.m` | `y = sinc_normalized(x)` | Computes `sin(pi*x)/(pi*x)`, with the value 1 at zero, without requiring Signal Processing Toolbox. |

`exercise_wave.m` defines its plotting and FFT helpers at the bottom of the same file. MATLAB makes those local functions available to that script; they are not separate files.

## Files the exercises need

- `StartMeUp_44100Hz_30s.wav` is included beside the scripts and is loaded by `exercise_wave.m`.
- `100m (1).mat` is not currently included. Obtain it from the course materials or select another MAT-file when prompted. The selected file must contain a variable named `val`; if it has multiple channels, the first channel is used.

## Notes

- `exercise_sinc.m` uses a target frequency resolution of `0.05` Hz. `function_FFT` pads to the next power of two while retaining all input samples, avoiding the previously requested 134-million-point FFT.
- `function_FFT` assumes the input samples are centered around time zero. It uses the matching frequency axis from `-Fs/2` to `Fs/2 - df`.
- `function_Fourier_Transform` still performs a direct transform, so a long signal can take time to compute. Processing frequency blocks limits temporary memory use.
- `exercise_electrocardiogram.m` needs a MAT-file containing `val` before it can plot an ECG.
