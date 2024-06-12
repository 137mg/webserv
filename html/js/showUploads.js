function deleteFile(filename)
{
	const filePath = 'uploads/' + filename;

	fetch(filePath,
	{
		method: 'DELETE'
	})
	.then(response =>
	{
		if (response.ok)
		{
			window.location.reload();
		}
		else
		{
			return response.text().then(text => {
				document.open();
				document.write(text);
				document.close();
			});
		}
	})
}