import os

class TestFormat:
    # Check empty files
    def test_empty_files(self):
        file_paths = TestFormat._get_file_paths('docs', [".md"])
        
        for path in file_paths:
            content = TestFormat._get_file_text(path)
            assert len(content) > 0, f"Path {path} has no content."
    
    
    def test_image_location(self):
        image_paths = TestFormat._get_file_paths('docs', [".png", ".jpg", ".jpeg"])
        
        for path in image_paths:
            assert "docs/assets" in path, "Images must be under /docs/assets"
    


    def _get_file_text(path):
        with open(path) as file:
            return file.read()
    
    def _get_file_paths(parent_dir, extensions):
        extensions = set(extensions)

        paths = []
        for root, dirs, files in os.walk(parent_dir):
            for file in files:
                if TestFormat._get_ext(file) in extensions:
                    paths.append(os.path.join(root, file))
        
        return paths

    def _get_ext(path):
        return os.path.splitext(path)[1]