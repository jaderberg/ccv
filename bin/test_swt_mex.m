% Max Jaderberg 7/1/13

% Tests out the swt_word_contours_mex.c file

% Compile
mex swt_word_contours_mex.c -L"/Users/jaderberg/Work/Utils/ccv_max/lib" -I"/Users/jaderberg/Work/Utils/ccv_max/lib" -lccv -ljpeg -lpng -lz -L/usr/X11/lib -lm -L/opt/local/lib -I/opt/local/include

im = imread('~/Desktop/ryan.jpg');
img = rgb2gray(im);

res.success = 0;

while ~res.success
    res = swt_word_contours_mex(img, 's', '3');
end

figure(1); clf;
imshow(im); hold on;
for i=1:size(res.words)
    r = rectangle('Position', res.words{i}.rect);
    set(r, 'edgecolor', 'r');
end
hold off;

figure(2); clf;
imshow(im); hold on;
for i=1:size(res.words)
    for j=1:size(res.words{i}.chars)
        if isempty(res.words{i}.chars{j})
            continue
        end
        r = rectangle('Position', res.words{i}.chars{j}.rect);
        set(r, 'edgecolor', 'b');
        plot(res.words{i}.chars{j}.center(1), res.words{i}.chars{j}.center(2), '+');
    end
end
hold off;

figure(3); clf;
M = zeros(size(img));
for i=1:size(res.words)
    for j=1:size(res.words{i}.chars)
        p = res.words{i}.chars{j}.S;
        M(p(1,:), p(2,:)) = 1;
    end
end
imshow(M);