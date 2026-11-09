function [G_FFT, frequency_samples_FFT] = function_FFT(g, Ts, deltaf)
%FUNCTION_FFT Approximate the CTFT of samples centered around t = 0.

if ~isvector(g) || isempty(g)
    error('function_FFT:InvalidSignal', 'g must be a nonempty vector.');
end
if ~isscalar(Ts) || ~isfinite(Ts) || Ts <= 0
    error('function_FFT:InvalidSamplingTime', 'Ts must be a positive finite scalar.');
end
if ~isscalar(deltaf) || ~isfinite(deltaf) || deltaf <= 0
    error('function_FFT:InvalidFrequencyStep', 'deltaf must be a positive finite scalar.');
end

% Honor the requested frequency resolution without ever truncating g.
requestedLength = ceil(1/(deltaf*Ts));
N_FFT = 2^nextpow2(max([2, numel(g), requestedLength]));
deltaf = 1/(N_FFT*Ts);

% Place the zero-time sample at the center before the FFT. This preserves
% the time origin for signals supplied on a symmetric time grid.
g = g(:);
g_centered = zeros(N_FFT, 1, 'like', g);
sourceCenter = floor(numel(g)/2) + 1;
destinationCenter = floor(N_FFT/2) + 1;
firstIndex = destinationCenter - sourceCenter + 1;
g_centered(firstIndex:firstIndex + numel(g) - 1) = g;

G_FFT = Ts*fftshift(fft(ifftshift(g_centered)));
frequency_samples_FFT = (-N_FFT/2:N_FFT/2-1)'*deltaf;
