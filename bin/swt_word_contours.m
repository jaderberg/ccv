% SWT_WORD_CONTOURS
%   res = SWT_WORD_CONTOURS(I) computes the locations and contours of words
%   using the Stroke Width Transform algorithm in CCV.
%   
%   The accepted options are:
%       h    min height [8]
%		H    max height [300]
%		a    min area [38]
%		s    canny size (must be an odd number) [3]
%		t    canny low thresh [124]
%		T    canny high thresh [204]
%		o    letter occlude thresh (cannot occlude more than -o other letters) [3]
%		r    maximum aspect ratio of a letter [8]
%		v    max inner class variance for grouping letters [0.83]
%		k    max thickness variance for grouping letters [1.5]
%		g    max height variance for grouping letters [1.7]
%		i    max intensity variance for grouping letters [31]
%		d    max distance variance for grouping letters [2.9]
%		I    max intersect variance for grouping letters [1.3]
%		l    max letters threshold for grouping letters [3]
%		e    max elongate variance for grouping letters [1.9]
%		b    breakdown textlines to words [1]
%		B    breakdown threshold [1.0]
%       M    number of cache rows [10000]
%       N    number of cache cols [500]
%
%   Max Jaderberg 8/1/13

function words = swt_word_contours(im, varargin)

    viz = true;
    
    words = {};
    
    % must be a gray image
    if numel(size(im)) == 3
        im = rgb2gray(im);
    end
    res = swt_word_contours_mex(im, varargin{:});
    
    if ~res.success
        return
    end
    
    for i=1:length(res.words)
        for j=1:length(res.words{i}.chars)
            id = res.words{i}.chars{j}.id;
            res.words{i}.chars{j}.S = res.contours{id};
            res.words{i}.chars{j}.rect = res.rectangles(:,id)';
            res.words{i}.chars{j}.center = res.centers(:,id)';
        end
    end
    words = res.words;
    clear res;
    
    if viz
        figure(42); clf;
        imshow(im); hold on;
        for i=1:size(words)
            r = rectangle('Position', words{i}.rect);
            set(r, 'edgecolor', 'r');
        end
        hold off;

        figure(43); clf;
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

        figure(44); clf;
        M = zeros(size(im, 1), size(im, 2));
        for i=1:size(words)
            for j=1:size(words{i}.chars)
                M(words{i}.chars{j}.S) = 1;
            end
        end
        imshow(M);
    end