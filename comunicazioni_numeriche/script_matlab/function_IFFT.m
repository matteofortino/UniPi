function [g_IFFT, time_samples_IFFT] = function_IFFT(G_FFT, Ts, deltaf)
%FUNCTION_IFFT Invert the centered spectrum returned by function_FFT.
% deltaf is retained for compatibility with the original function call.

if ~isvector(G_FFT) || isempty(G_FFT)
    error('function_IFFT:InvalidSpectrum', 'G_FFT must be a nonempty vector.');
end
if ~isscalar(Ts) || ~isfinite(Ts) || Ts <= 0
    error('function_IFFT:InvalidSamplingTime', 'Ts must be a positive finite scalar.');
end
if ~isscalar(deltaf) || ~isfinite(deltaf) || deltaf <= 0
    error('function_IFFT:InvalidFrequencyStep', 'deltaf must be a positive finite scalar.');
end

N_FFT = numel(G_FFT);
if mod(N_FFT, 2) ~= 0
    error('function_IFFT:InvalidSpectrumLength', 'G_FFT must have an even number of samples.');
end

G_FFT = G_FFT(:);
g_IFFT = fftshift(ifft(ifftshift(G_FFT)))/Ts;
time_samples_IFFT = (-N_FFT/2:N_FFT/2-1)'*Ts;
