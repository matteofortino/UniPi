
function [G, frequency_samples] = function_Fourier_Transform(g, time_samples, Ts, deltaf, durF)
%FUNCTION_FOURIER_TRANSFORM Compute a sampled continuous-time Fourier transform.

if numel(g) ~= numel(time_samples)
    error('function_Fourier_Transform:SizeMismatch', ...
        'g and time_samples must contain the same number of samples.');
end
if ~isscalar(Ts) || ~isfinite(Ts) || Ts <= 0
    error('function_Fourier_Transform:InvalidSamplingTime', ...
        'Ts must be a positive finite scalar.');
end
if ~isscalar(deltaf) || ~isfinite(deltaf) || deltaf <= 0
    error('function_Fourier_Transform:InvalidFrequencyStep', ...
        'deltaf must be a positive finite scalar.');
end
if ~isscalar(durF) || ~isfinite(durF) || durF < 0
    error('function_Fourier_Transform:InvalidFrequencyRange', ...
        'durF must be a nonnegative finite scalar.');
end

frequency_samples = -durF:deltaf:durF;
time_samples = reshape(time_samples, 1, []);
g = reshape(g, [], 1);

% Evaluate the DFT in frequency blocks to avoid allocating one large dense
% frequency-by-time matrix (especially important for long ECG records).
maxMatrixElements = 2e6;
blockSize = max(1, floor(maxMatrixElements/max(1, numel(time_samples))));
blockSize = min(blockSize, numel(frequency_samples));
G = complex(zeros(1, numel(frequency_samples)));

for firstIndex = 1:blockSize:numel(frequency_samples)
    lastIndex = min(firstIndex + blockSize - 1, numel(frequency_samples));
    frequencyBlock = frequency_samples(firstIndex:lastIndex);
    arg = 2*pi*frequencyBlock'*time_samples;
    G(firstIndex:lastIndex) = Ts*(exp(-1i*arg)*g).';
end
end
