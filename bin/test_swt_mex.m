% Max Jaderberg 7/1/13

% Tests out the swt_word_contours_mex.c file

% Compile
mex swt_word_contours_mex.c -L"/Users/jaderberg/Work/Utils/ccv_max/lib" -I"/Users/jaderberg/Work/Utils/ccv_max/lib" -lccv -ljpeg -lpng -lz -L/usr/X11/lib -lm -L/opt/local/lib -I/opt/local/include

im = imread('~/Desktop/ryan.jpg');

res.success = 0;

words = swt_word_contours(im);

figure(1); clf;
imshow(im); hold on;
for i=1:size(words)
    r = rectangle('Position', words{i}.rect);
    set(r, 'edgecolor', 'r');
end
hold off;

figure(2); clf;
imshow(im); hold on;
for i=1:size(words)
    for j=1:size(words{i}.chars)
        if isempty(words{i}.chars{j})
            continue
        end
        r = rectangle('Position', words{i}.chars{j}.rect);
        set(r, 'edgecolor', 'b');
        plot(words{i}.chars{j}.center(1), words{i}.chars{j}.center(2), '+');
    end
end
hold off;

figure(3); clf;
M = zeros(size(img));
for i=1:size(words)
    for j=1:size(words{i}.chars)
        M(words{i}.chars{j}.S) = 1;
    end
end
imshow(M);